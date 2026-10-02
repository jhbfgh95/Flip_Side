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
#include "TimerManager.h"

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


	// boss_gimmick(id=2, "독") param_int_a = 독 지속 턴수
	const int32 Duration = GimmickData.ParamIntA > 0 ? GimmickData.ParamIntA : 2;
	const FBossHUDData PatternData = Boss->GetBossHUDData();

	for (ACoinActor* Coin : AllHitCoins)
	{
		if (!IsValid(Coin)) continue;
		UComponent_Status* StatusComp = Coin->FindComponentByClass<UComponent_Status>();
		if (!IsValid(StatusComp)) continue;
		FStatusEffectInstance Poison;
		Poison.BuffTypeID = DebuffTypeID::Poison;
		Poison.Polarity = EStatusPolarity::Debuff;
		Poison.RemainingTurns = Duration;
		Poison.SourceType = EStatusEffectSourceType::Boss;
		Poison.SourceDataID = Boss->GetBossID();
		Poison.SourcePatternIndex = PatternData.PatternDisplayIndex > 0 ? PatternData.PatternDisplayIndex - 1 : INDEX_NONE;
		Poison.SourcePatternIcon = PatternData.PatternIcon;
		// 공통 디버프 저장소에서 재적용 시 갱신하며, 코인 재생성 시에도 복원합니다.
		StatusComp->AddStatusEffect(Poison);
	}
}

void UBossGimmick_Poison::OnPlayerPhaseStart(ABossActor* Boss)
{
	if (!IsValid(Boss)) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	const int32 PoisonDamage = GimmickData.ParamFloatA > 0.f ? static_cast<int32>(GimmickData.ParamFloatA) : 1;

	const TWeakObjectPtr<ABossActor> WeakBoss = Boss;
	const TWeakObjectPtr<UBossGimmick_Poison> WeakThis = this;
	World->GetTimerManager().SetTimer(PoisonTimerHandle, [WeakThis, WeakBoss, PoisonDamage]()
	{
		ABossActor* ActiveBoss = WeakBoss.Get();
		if (!WeakThis.IsValid() || !IsValid(ActiveBoss) || ActiveBoss->GetCurrentHP() <= 0) return;
		UWorld* ActiveWorld = ActiveBoss->GetWorld();
		UGridManagerSubsystem* ActiveGrid = IsValid(ActiveWorld) ? ActiveWorld->GetSubsystem<UGridManagerSubsystem>() : nullptr;
		if (!IsValid(ActiveGrid)) return;
		// 타이머를 만들 때 고정하지 않고 매 틱 조회하여 HP 감소 시 초기화를 즉시 반영합니다.
		const int32 FinalDamage = ActiveBoss->GetDamageWithPillarBonus(PoisonDamage);
		TArray<FCoinOnGridInfo> CurrentCoins;
		ActiveGrid->CollectOccupiedCoins(CurrentCoins);
		for (const FCoinOnGridInfo& Info : CurrentCoins)
		{
			if (!IsValid(Info.CoinActor)) continue;
			UComponent_Status* StatusComp = Info.CoinActor->FindComponentByClass<UComponent_Status>();
			if (!IsValid(StatusComp)) continue;
			// 이전 턴 액터 참조 대신 현재 코인에 복원된 독 상태를 확인합니다.
			for (const FStatusEffectInstance& Effect : StatusComp->GetStatusEffects())
			{
				if (Effect.BuffTypeID == DebuffTypeID::Poison && Effect.RemainingTurns > 0 &&
					Effect.SourceType == EStatusEffectSourceType::Boss && Effect.SourceDataID == ActiveBoss->GetBossID())
				{
					StatusComp->ApplyDamage(FinalDamage, ActiveBoss);
					break;
				}
			}
		}
	},
	5.f, true);
}

void UBossGimmick_Poison::OnPlayerPhaseEnd(ABossActor* Boss)
{
	if (!IsValid(Boss)) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(PoisonTimerHandle);

	// 지속 턴 감소는 코인 상태 저장 시 공통 디버프 저장소에서 한 번만 처리합니다.
}
