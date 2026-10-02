#include "BossPatternBase.h"
#include "BossGimmickBase.h"
#include "BossActor.h"
#include "BossActor_RoleTarget.h"
#include "BossManagerSubsystem.h"
#include "GridManagerSubsystem.h"
#include "GridActor.h"
#include "BattleLevelActingWSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
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

	// 원뿔 강타는 (4,4)에서 시작해 Y가 감소하는 방향으로 펼칩니다.
	// 현재 플레이 영역의 마지막 행은 Y=4입니다.
	if (Spec.Pattern == EAttackAreaPattern::ConeFromSide && Spec.AnchorMode == EAreaAnchor::UseIndex)
	{
		Spec.Index = 4;
		Spec.Side = EAreaSide::Down;
	}

	// CircleOnCell UseAnchorCell 모드면 AnchorCell 랜덤 선택
	if (Spec.Pattern == EAttackAreaPattern::CircleOnCell && Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
	{
		const int32 Radius = FMath::Max(1, Spec.ParamA);
		const int32 PlayableHeight = GridMgr->GetBossAreaStartY();
		if (GridMgr->GridXSize <= 0 || PlayableHeight <= 0)
		{
			OutCells.Reset();
			return;
		}
		// 보스 영역을 제외하고 전체 범위가 들어가는 중심을 선택합니다.
		// 영역보다 큰 패턴은 중앙에 두어 역전된 랜덤 범위를 방지합니다.
		const int32 MarginX = FMath::Min(Radius, (GridMgr->GridXSize - 1) / 2);
		const int32 MarginY = FMath::Min(Radius, (PlayableHeight - 1) / 2);
		Spec.AnchorCell.GridX = FMath::RandRange(MarginX, GridMgr->GridXSize - 1 - MarginX);
		Spec.AnchorCell.GridY = FMath::RandRange(MarginY, PlayableHeight - 1 - MarginY);
	}

	// CrossOnCell UseAnchorCell 모드면 AnchorCell 랜덤 선택
	// (십자는 앵커 기준 상하좌우로 대칭 확장되므로, 보스 자기 영역까지 포함해서 잘리지 않도록 Y 상한도 보스 영역 시작 지점 앞으로 제한)
	if (Spec.Pattern == EAttackAreaPattern::CrossOnCell && Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
	{
		const int32 HalfX = FMath::Max(0, Spec.ParamA);
		const int32 HalfY = FMath::Max(0, Spec.ParamB);
		const int32 MaxY = FMath::Max(HalfY, GridMgr->GetBossAreaStartY() - 1 - HalfY);
		Spec.AnchorCell.GridX = FMath::RandRange(HalfX, GridMgr->GridXSize - 1 - HalfX);
		Spec.AnchorCell.GridY = FMath::RandRange(HalfY, MaxY);
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
		if (Spec.Side == EAreaSide::Up)
		{
			// 패턴이 보스 자기 영역(뒤쪽 3x9)까지 침범하지 않도록 상한을 보스 영역 시작 지점 앞으로 제한
			MaxY = GridMgr->GetBossAreaStartY() - 1 - Depth;
		}
		else if (Spec.Side == EAreaSide::Down) { MinY = Depth; }

		Spec.AnchorCell.GridX = FMath::RandRange(MinX, MaxX);
		Spec.AnchorCell.GridY = FMath::RandRange(FMath::Max(0, MinY), FMath::Max(0, MaxY));
	}

	// RoleTarget 패턴: Role 미확정이면 빈 배열, 확정되면 역할군 코인 위치 수집
	if (PatternData.IsValidIndex(PatternNum) && PatternData[PatternNum].GimmickType == EBossGimmickType::RoleTarget)
	{
		if (ABossActor_RoleTarget* RoleTargetBoss = Cast<ABossActor_RoleTarget>(Boss))
		{
			if (RoleTargetBoss->IsRoleLocked())
			{
				const EWeaponClass TargetRole = RoleTargetBoss->GetLockedRoleClass();
				TArray<FCoinOnGridInfo> OccupiedCoins;
				GridMgr->CollectOccupiedCoins(OccupiedCoins);
				for (const FCoinOnGridInfo& Info : OccupiedCoins)
				{
					if (IsValid(Info.CoinActor) && Info.CoinActor->GetWeaponType() == TargetRole)
					{
						OutCells.Add(Info.GridXY);
					}
				}
			}
			return; // 미확정이면 빈 배열 반환
		}
	}

	GridMgr->BuildBossAttackCells(Spec, OutCells);
}

void UBossPatternBase::ExecutePattern(
	ABossActor* Boss,
	FBossPhaseContext& Context,
	const TArray<ACoinActor*>& InLockedTargets,
	const TArray<ABase_OtherActor*>& InLockedOthers)
{
	if (!IsValid(Boss) || Boss->IsStunned()) return;

	const int32 PatternNum = Context.CurrentPatternIndex;
	const bool bNoDamage = PatternData.IsValidIndex(PatternNum) && PatternData[PatternNum].bNoDamage;

	UBossGimmickBase* Gimmick = Boss->GetActiveGimmick();

    if (!bNoDamage && !Context.bSkipAttack && !Boss->IsBlinded())
	{
		int32 FinalDamage = FMath::RoundToInt((Context.BaseDamage + Context.BonusDamage) * Context.DamageMultiplier);

		if (Gimmick)
			Gimmick->OnDamageCalculate(Boss, FinalDamage);

		ExecuteDamage(InLockedTargets, InLockedOthers, Boss, FinalDamage);
	}
	else if (bNoDamage && PatternData.IsValidIndex(PatternNum) && PatternData[PatternNum].GimmickType == EBossGimmickType::Shield && PatternData[PatternNum].ShieldHeal > 0)
	{
		const int32 HealAmount = FMath::RoundToInt(PatternData[PatternNum].ShieldHeal * Boss->GetStageMultiplierStat());
		Boss->ApplyShieldHeal(HealAmount, Boss);
	}

	if (Gimmick)
		Gimmick->OnPatternExecute(Boss, Context.LockedCells, InLockedTargets, InLockedOthers);
}

void UBossPatternBase::ExecuteDamage(const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers, ABossActor* Boss, int32 Damage)
{
    // BP에서 피해 함수를 직접 호출하는 경로도 동일한 CC 규칙을 적용합니다.
    if (!IsValid(Boss) || Boss->IsBlinded() || Boss->IsStunned()) return;
    const int32 FinalDamage = Boss->GetDamageWithPillarBonus(Damage);
    for (ACoinActor* Coin : LockedTargets)
    {
        if (!IsValid(Coin)) continue;

        UComponent_Status* StatusComp = Coin->FindComponentByClass<UComponent_Status>();
        if (!StatusComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("[BossPattern] CoinID=%d has no Component_Status"), Coin->GetCoinID());
            continue;
        }

        StatusComp->ApplyDamage(FinalDamage, Boss);

    }

    for (ABase_OtherActor* Other : LockedOthers)
    {
        if (!IsValid(Other)) continue;
        Other->ApplyDamage(FinalDamage, Boss);
    }


}
void UBossPatternBase::PlayPatternEffect_Implementation(int32 PatternNum, FVector EffectLocation)
{
	if (!PatternData.IsValidIndex(PatternNum)) return;
	if (PatternData[PatternNum].PatternEffect.IsNull()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	UBattleLevelActingWSubsystem* ActingMgr = World->GetSubsystem<UBattleLevelActingWSubsystem>();
	UBossManagerSubsystem* BossMgr = World->GetSubsystem<UBossManagerSubsystem>();
	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!ActingMgr || !BossMgr || !GridMgr) return;

	UNiagaraSystem* Effect = PatternData[PatternNum].PatternEffect.LoadSynchronous();
	if (!Effect) return;

	const FBossPatternBattleData& Data = PatternData[PatternNum];

	// LockedCells → 월드 좌표 변환
	const TArray<FGridPoint>& LockedCells = BossMgr->GetCurrentPhaseLockedCells();
	TArray<FVector> CellLocations;
	for (const FGridPoint& Cell : LockedCells)
	{
		AGridActor* GridActor = GridMgr->GetGridActor(Cell);
		if (!IsValid(GridActor)) continue;
		FVector2D XY = GridActor->GetGridWorldXY();
		CellLocations.Add(FVector(XY.X, XY.Y, GridActor->GetActorLocation().Z));
	}

	// AnchorCell 좌표
	// AnchorCell은 런타임에 랜덤 결정되므로 LockedCells 중심을 사용
	FVector AnchorLocation = FVector::ZeroVector;
	if (CellLocations.Num() > 0)
	{
		for (const FVector& Loc : CellLocations)
			AnchorLocation += Loc;
		AnchorLocation /= CellLocations.Num();
	}

	ActingMgr->PlayBossVFX(Effect, Data.PatternEffectTarget, Data.PatternScale, CellLocations, AnchorLocation);
}
