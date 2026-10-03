#include "BossGimmick_Poison.h"
#include "BossActor.h"
#include "CoinActor.h"
#include "Component_Status.h"
#include "Actors/DebuffComponent.h"
#include "GridManagerSubsystem.h"
#include "Actors/Others/Base_OtherActor.h"
#include "CoinDataTypes.h"
#include "FlipSide_Enum.h"
#include "Engine/World.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"

void UBossGimmick_Poison::OnPatternExecute(
	ABossActor* Boss,
	const TArray<FGridPoint>& LockedCells,
	const TArray<ACoinActor*>& LockedTargets,
	const TArray<ABase_OtherActor*>& LockedOthers)
{
	if (!IsValid(Boss)) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr) return;

	// ValidLockedTargets 외에 LockedCells에 있던 코인도 포함 (데미지 후 셀 이탈한 코인)
	TArray<ACoinActor*> AllHitCoins = LockedTargets;
	TArray<FCoinOnGridInfo> OccupiedCoins;
	GridMgr->CollectOccupiedCoins(OccupiedCoins);
	for (const FCoinOnGridInfo& Info : OccupiedCoins)
	{
		if (!IsValid(Info.CoinActor)) continue;
		bool bAlreadyIn = AllHitCoins.Contains(Info.CoinActor);
		if (bAlreadyIn) continue;
		for (const FGridPoint& Cell : LockedCells)
		{
			if (Info.GridXY.GridX == Cell.GridX && Info.GridXY.GridY == Cell.GridY)
			{
				AllHitCoins.Add(Info.CoinActor);
				break;
			}
		}
	}


	// 낙인은 지속 턴 제한 없이 끝까지 유지됩니다(정화 물약으로만 해제). 턴 차감으로 사라지지 않도록 상한값을 둡니다.
	constexpr int32 BrandDuration = 999;
	const UBattleManagerWSubsystem* Battle = World->GetSubsystem<UBattleManagerWSubsystem>();
	const int32 AppliedTurn = IsValid(Battle) ? Battle->GetTurnCount() : 0;
	const FBossHUDData PatternData = Boss->GetBossHUDData();

	for (ACoinActor* Coin : AllHitCoins)
	{
		if (!IsValid(Coin)) continue;
		UComponent_Status* StatusComp = Coin->FindComponentByClass<UComponent_Status>();
		if (!IsValid(StatusComp)) continue;
		FStatusEffectInstance Brand;
		Brand.BuffTypeID = DebuffTypeID::Poison;
		Brand.Polarity = EStatusPolarity::Debuff;
		Brand.RemainingTurns = BrandDuration;
		Brand.RuntimeValue = AppliedTurn; // 찍힌 턴. 지난 턴 수 = 현재 턴 - 찍힌 턴
		Brand.SourceType = EStatusEffectSourceType::Boss;
		Brand.SourceDataID = Boss->GetBossID();
		Brand.SourcePatternIndex = PatternData.PatternDisplayIndex > 0 ? PatternData.PatternDisplayIndex - 1 : INDEX_NONE;
		Brand.SourcePatternIcon = PatternData.PatternIcon;
		// DebuffComponent가 낙인은 덮어쓰지 않고 개별 인스턴스로 추가하며, 코인 재생성 시에도 복원합니다.
		StatusComp->AddStatusEffect(Brand);
	}
}

void UBossGimmick_Poison::OnPlayerPhaseEnd(ABossActor* Boss)
{
	if (!IsValid(Boss)) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	const UBattleManagerWSubsystem* Battle = World->GetSubsystem<UBattleManagerWSubsystem>();
	if (!IsValid(GridMgr) || !IsValid(Battle)) return;

	// 보스 페이즈 진입 시점(새 패턴 실행 전)에 기존 낙인 피해를 줍니다.
	const int32 CurrentTurn = Battle->GetTurnCount();
	// 낙인 피해는 기둥 보너스를 더하지 않습니다. 보너스까지 턴 수에 곱해지면 피해가 의도보다 훨씬 커집니다.
	const int32 BaseDamage = GimmickData.ParamFloatA > 0.f ? static_cast<int32>(GimmickData.ParamFloatA) : 1;

	TArray<FCoinOnGridInfo> CurrentCoins;
	GridMgr->CollectOccupiedCoins(CurrentCoins);
	for (const FCoinOnGridInfo& Info : CurrentCoins)
	{
		if (!IsValid(Info.CoinActor)) continue;
		UComponent_Status* StatusComp = Info.CoinActor->FindComponentByClass<UComponent_Status>();
		if (!IsValid(StatusComp)) continue;

		// 코인에 붙은 모든 낙인의 지난 턴 수를 합산합니다.
		int64 TotalElapsedTurns = 0;
		for (const FStatusEffectInstance& Effect : StatusComp->GetStatusEffects())
		{
			if (Effect.BuffTypeID == DebuffTypeID::Poison && Effect.RemainingTurns > 0 &&
				Effect.SourceType == EStatusEffectSourceType::Boss && Effect.SourceDataID == Boss->GetBossID())
			{
				TotalElapsedTurns += BrandDebuff::ElapsedTurns(Effect, CurrentTurn);
			}
		}
		if (TotalElapsedTurns <= 0) continue;

		const int32 FinalDamage = static_cast<int32>(FMath::Clamp<int64>(static_cast<int64>(BaseDamage) * TotalElapsedTurns, 0, MAX_int32));
		UE_LOG(LogTemp, Log, TEXT("[Brand] Turn=%d Coin=%s BaseDamage=%d TotalElapsedTurns=%lld FinalDamage=%d HP=%d"),
			CurrentTurn, *GetNameSafe(Info.CoinActor), BaseDamage, TotalElapsedTurns, FinalDamage, StatusComp->GetHP());
		StatusComp->ApplyDamage(FinalDamage, Boss);
	}
}
