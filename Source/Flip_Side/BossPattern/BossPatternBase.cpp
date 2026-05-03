#include "BossPatternBase.h"
#include "BossGimmickBase.h"
#include "BossActor.h"
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

	// Row/Column UseIndex 모드면 랜덤 행/열 선택
	if (Spec.AnchorMode == EAreaAnchor::UseIndex)
	{
		if (Spec.Pattern == EAttackAreaPattern::Row)
		{
			Spec.Index = FMath::RandRange(0, GridMgr->GridYSize - 1);
		}
		else if (Spec.Pattern == EAttackAreaPattern::Column)
		{
			Spec.Index = FMath::RandRange(0, GridMgr->GridXSize - 1);
		}
	}

	// ConeFromSide UseIndex 모드면 Side를 Up/Down 랜덤 선택
	if (Spec.Pattern == EAttackAreaPattern::ConeFromSide && Spec.AnchorMode == EAreaAnchor::UseIndex)
	{
		Spec.Side = FMath::RandBool() ? EAreaSide::Up : EAreaSide::Down;
	}

	// CircleOnCell UseAnchorCell 모드면 AnchorCell 랜덤 선택
	if (Spec.Pattern == EAttackAreaPattern::CircleOnCell && Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
	{
		const int32 Radius = FMath::Max(1, Spec.ParamA);
		Spec.AnchorCell.GridX = FMath::RandRange(Radius, GridMgr->GridXSize - 1 - Radius);
		Spec.AnchorCell.GridY = FMath::RandRange(Radius, GridMgr->GridYSize - 1 - Radius);
	}

	// CrossOnCell UseAnchorCell 모드면 AnchorCell 랜덤 선택
	if (Spec.Pattern == EAttackAreaPattern::CrossOnCell && Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
	{
		const int32 HalfX = FMath::Max(0, Spec.ParamA);
		const int32 HalfY = FMath::Max(0, Spec.ParamB);
		Spec.AnchorCell.GridX = FMath::RandRange(HalfX, GridMgr->GridXSize - 1 - HalfX);
		Spec.AnchorCell.GridY = FMath::RandRange(HalfY, GridMgr->GridYSize - 1 - HalfY);
	}

	// RectFromCell UseAnchorCell 모드면 AnchorCell 랜덤 선택 (패턴이 그리드 밖으로 안 나가도록 범위 제한)
	if (Spec.Pattern == EAttackAreaPattern::RectFromCell && Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
	{
		const int32 HalfW = Spec.ParamA / 2;
		const int32 Depth = FMath::Max(1, Spec.ParamB);

		const int32 MinX = HalfW;
		const int32 MaxX = FMath::Max(MinX, GridMgr->GridXSize - 1 - HalfW);

		int32 MinY = 0;
		int32 MaxY = GridMgr->GridYSize - 1;
		if (Spec.Side == EAreaSide::Up)   { MaxY = GridMgr->GridYSize - 1 - Depth; }
		else if (Spec.Side == EAreaSide::Down) { MinY = Depth; }

		Spec.AnchorCell.GridX = FMath::RandRange(MinX, MaxX);
		Spec.AnchorCell.GridY = FMath::RandRange(FMath::Max(0, MinY), FMath::Max(0, MaxY));
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
	if (!Boss) return;

	const bool bNoDamage = PatternData.IsValidIndex(PatternNum) && PatternData[PatternNum].bNoDamage;

	UBossGimmickBase* Gimmick = Boss->GetActiveGimmick();
	UE_LOG(LogTemp, Warning, TEXT("[ExecutePattern] PatternNum=%d, bNoDamage=%d, Targets=%d, ActiveGimmick=%s"),
		PatternNum, bNoDamage, InLockedTargets.Num(),
		Gimmick ? *Gimmick->GetClass()->GetName() : TEXT("None"));

	if (!bNoDamage)
	{
		int32 FinalDamage = Boss->GetAttackPoint() + BonusDamage;

		if (Gimmick)
			Gimmick->OnDamageCalculate(Boss, FinalDamage);

		ExecuteDamage(InLockedTargets, InLockedOthers, Boss, FinalDamage);
	}
	else if (PatternData[PatternNum].GimmickType == EBossGimmickType::Shield && PatternData[PatternNum].ShieldHeal > 0)
	{
		const int32 HealAmount = FMath::RoundToInt(PatternData[PatternNum].ShieldHeal * Boss->GetStageMultiplierStat());
		Boss->ApplyShieldHeal(HealAmount, Boss);
		UE_LOG(LogTemp, Warning, TEXT("[BossPattern] 방어막 회복 %d (원본 %d)"), HealAmount, PatternData[PatternNum].ShieldHeal);
	}

	if (Gimmick)
		Gimmick->OnPatternExecute(Boss, InLockedCells, InLockedTargets, InLockedOthers);
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