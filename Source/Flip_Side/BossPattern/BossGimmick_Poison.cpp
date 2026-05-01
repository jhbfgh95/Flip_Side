#include "BossGimmick_Poison.h"
#include "BossActor.h"
#include "CoinActor.h"
#include "Component_Status.h"
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

	PoisonedCoins.Reset();
	for (ACoinActor* Coin : LockedTargets)
	{
		if (!IsValid(Coin)) continue;
		PoisonedCoins.Add(Coin);
	}
}

void UBossGimmick_Poison::OnPlayerTurnStart(ABossActor* Boss)
{
	if (!Boss || PoisonedCoins.Num() == 0) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	const float DamagePerSec = GimmickData.ParamFloatA > 0.f ? GimmickData.ParamFloatA : 5.f;

	World->GetTimerManager().SetTimer(PoisonTimerHandle, [this, Boss, DamagePerSec]()
	{
		for (TWeakObjectPtr<ACoinActor> WeakCoin : PoisonedCoins)
		{
			if (!WeakCoin.IsValid()) continue;

			UComponent_Status* StatusComp = WeakCoin->FindComponentByClass<UComponent_Status>();
			if (StatusComp)
			{
				StatusComp->ApplyDamage(static_cast<int32>(DamagePerSec), Boss);
			}
		}
	},
	1.f, true);
}

void UBossGimmick_Poison::OnPlayerTurnEnd(ABossActor* Boss)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(PoisonTimerHandle);
	PoisonedCoins.Reset();
}
