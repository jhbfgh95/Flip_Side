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

	PoisonedCoins.Reset();

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

	for (ACoinActor* Coin : AllHitCoins)
	{
		if (!IsValid(Coin)) continue;
		PoisonedCoins.Add(Coin);
		UE_LOG(LogTemp, Warning, TEXT("[Poison] CoinID=%d 독 등록"), Coin->GetCoinID());
	}
}

void UBossGimmick_Poison::OnPlayerTurnStart(ABossActor* Boss)
{
	if (!Boss || PoisonedCoins.Num() == 0) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	const int32 PoisonDamage = GimmickData.ParamFloatA > 0.f ? static_cast<int32>(GimmickData.ParamFloatA) : 1;
	UE_LOG(LogTemp, Warning, TEXT("[Poison] 독 타이머 시작 - 대상=%d명, 데미지=%d / 5초"), PoisonedCoins.Num(), PoisonDamage);

	World->GetTimerManager().SetTimer(PoisonTimerHandle, [this, Boss, PoisonDamage]()
	{
		for (TWeakObjectPtr<ACoinActor> WeakCoin : PoisonedCoins)
		{
			if (!WeakCoin.IsValid()) continue;

			UComponent_Status* StatusComp = WeakCoin->FindComponentByClass<UComponent_Status>();
			if (StatusComp)
			{
				const int32 PrevHP = StatusComp->GetHP();
				StatusComp->ApplyDamage(PoisonDamage, Boss);
				UE_LOG(LogTemp, Warning, TEXT("[Poison] CoinID=%d 독 데미지 %d — HP %d -> %d"),
					WeakCoin->GetCoinID(), PoisonDamage, PrevHP, StatusComp->GetHP());
			}
		}
	},
	5.f, true);
}

void UBossGimmick_Poison::OnPlayerTurnEnd(ABossActor* Boss)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(PoisonTimerHandle);
	PoisonedCoins.Reset();
}
