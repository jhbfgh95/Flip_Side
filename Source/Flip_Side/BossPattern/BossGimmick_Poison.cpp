#include "BossGimmick_Poison.h"
#include "BossActor.h"
#include "CoinActor.h"
#include "Component_Status.h"
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
	if (!Boss) return;

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

	UE_LOG(LogTemp, Warning, TEXT("[Poison] OnPatternExecute - LockedTargets=%d, AllHitCoins=%d"), LockedTargets.Num(), AllHitCoins.Num());

	// boss_gimmick(id=2, "독") param_int_a = 독 지속 턴수
	const int32 Duration = GimmickData.ParamIntA > 0 ? GimmickData.ParamIntA : 2;

	for (ACoinActor* Coin : AllHitCoins)
	{
		if (!IsValid(Coin)) continue;
		// 이미 독이 걸려있어도 갱신(기존 잔여 턴수를 지우고 새로 적용)
		PoisonedCoins.Add(Coin, Duration);
		UE_LOG(LogTemp, Warning, TEXT("[Poison] CoinID=%d 독 등록 (%d턴)"), Coin->GetCoinID(), Duration);
	}
}

void UBossGimmick_Poison::OnPlayerPhaseStart(ABossActor* Boss)
{
	if (!Boss || PoisonedCoins.Num() == 0) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	const int32 PoisonDamage = GimmickData.ParamFloatA > 0.f ? static_cast<int32>(GimmickData.ParamFloatA) : 1;
	UE_LOG(LogTemp, Warning, TEXT("[Poison] 독 타이머 시작 - 대상=%d명, 데미지=%d / 5초"), PoisonedCoins.Num(), PoisonDamage);

	World->GetTimerManager().SetTimer(PoisonTimerHandle, [this, Boss, PoisonDamage]()
	{
		for (const TPair<TWeakObjectPtr<ACoinActor>, int32>& Pair : PoisonedCoins)
		{
			if (!Pair.Key.IsValid()) continue;

			ACoinActor* Coin = Pair.Key.Get();
			UComponent_Status* StatusComp = Coin->FindComponentByClass<UComponent_Status>();
			if (StatusComp)
			{
				const int32 PrevHP = StatusComp->GetHP();
				StatusComp->ApplyDamage(PoisonDamage, Boss);
				UE_LOG(LogTemp, Warning, TEXT("[Poison] CoinID=%d 독 데미지 %d — HP %d -> %d"),
					Coin->GetCoinID(), PoisonDamage, PrevHP, StatusComp->GetHP());
			}
		}
	},
	5.f, true);
}

void UBossGimmick_Poison::OnPlayerPhaseEnd(ABossActor* Boss)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(PoisonTimerHandle);

	// 한 턴이 지났으므로 잔여 지속 턴수를 깎고, 0이 되면 독 해제 (남은 코인은 다음 턴에도 계속 틱)
	for (auto It = PoisonedCoins.CreateIterator(); It; ++It)
	{
		if (!It->Key.IsValid())
		{
			It.RemoveCurrent();
			continue;
		}

		if (--It->Value <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Poison] CoinID=%d 독 해제"), It->Key->GetCoinID());
			It.RemoveCurrent();
		}
	}
}
