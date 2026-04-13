#include "BossPatternBase.h"
#include "GridManagerSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Component_Status.h"
#include "CoinActor.h"
#include "Actors/Others/Base_OtherActor.h"

void UBossPatternBase::BuildTargetCells(
	ABossActor* Boss,
	TArray<FGridPoint>& OutCells, int32 PatternNum)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr)
	{
		return;
	}

	
	FAttackAreaSpec Spec;
	
	if(PatternData.IsValidIndex(PatternNum))
	{
		Spec = PatternData[PatternNum].PatternSpec;
	}

	GridMgr->BuildBossAttackCells(Spec, OutCells);
}

void UBossPatternBase::ExecutePattern(
	ABossActor* Boss,
	const TArray<FGridPoint>& InLockedCells,
	const TArray<ACoinActor*>& InLockedTargets,
	const TArray<ABase_OtherActor*>& InLockedOthers,
	int32 PatternNum)
{
}

void UBossPatternBase::ExecuteDamage(const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers, ABossActor* Boss, int32 Damage)
{
    for (ACoinActor* Coin : LockedTargets)
    {
        if (!IsValid(Coin)) continue;

        UComponent_Status* StatusComp = Coin->FindComponentByClass<UComponent_Status>();
        if (!StatusComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("[BossPattern] CoinID=%d has no Component_Status"), Coin->GetCoinID());
            continue;
        }

        const int32 PrevHP = StatusComp->GetHP();
        const int32 NextHP = PrevHP - FMath::Max(0, Damage);

        StatusComp->ApplyDamage(Damage, Boss);

        UE_LOG(LogTemp, Log, TEXT("[BossPattern] Damage Applied - CoinID=%d HP %d -> %d"),
            Coin->GetCoinID(), PrevHP, FMath::Max(0, NextHP));
	}

    for (ABase_OtherActor* Other : LockedOthers)
    {
        if (!IsValid(Other)) continue;
        Other->ApplyDamage(Damage, Boss);
    }
}
void UBossPatternBase::PlayPatternEffect_Implementation(int32 PatternNum, FVector EffectLocation)
{
	if(!PatternData.IsValidIndex(PatternNum)) return;
	if(!PatternData[PatternNum].PatternEffect.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Naiagara On"));

		UNiagaraSystem* DefaultEffect = PatternData[PatternNum].PatternEffect.LoadSynchronous();

		if (DefaultEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), 
				DefaultEffect, 
				EffectLocation, 
				FRotator::ZeroRotator,
				PatternData[PatternNum].PatternScale
			);
		}
	}

}