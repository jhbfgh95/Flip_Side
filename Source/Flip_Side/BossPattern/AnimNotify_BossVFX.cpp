#include "AnimNotify_BossVFX.h"
#include "BossManagerSubsystem.h"
#include "BattleLevelActingWSubsystem.h"
#include "GridManagerSubsystem.h"
#include "GridActor.h"
#include "AttackAreaTypes.h"
#include "NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_BossVFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	UWorld* World = MeshComp->GetWorld();
	if (!World) return;

	UBossManagerSubsystem* BossMgr = World->GetSubsystem<UBossManagerSubsystem>();
	UBattleLevelActingWSubsystem* ActingMgr = World->GetSubsystem<UBattleLevelActingWSubsystem>();
	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!BossMgr || !ActingMgr || !GridMgr) return;

	const int32 PatternIndex = BossMgr->GetCurrentTurnPatternIndex();
	UBossPatternBase* Pattern = BossMgr->GetCurrentTurnPattern();
	if (!Pattern || PatternIndex == INDEX_NONE) return;

	FBossPatternBattleData PatternData;
	ABossActor* Boss = BossMgr->GetCurrentBoss();
	if (!Boss || !Boss->GetPatternData(PatternIndex, PatternData)) return;

	// Effect: OverrideEffect 우선, 없으면 패턴 데이터 사용
	UNiagaraSystem* Effect = nullptr;
	if (!OverrideEffect.IsNull())
	{
		Effect = OverrideEffect.LoadSynchronous();
	}
	else if (!PatternData.PatternEffect.IsNull())
	{
		Effect = PatternData.PatternEffect.LoadSynchronous();
	}
	if (!Effect) return;

	// Scale: ScaleOverride가 ZeroVector면 패턴 데이터 사용
	const FVector Scale = ScaleOverride.IsZero() ? PatternData.PatternScale : ScaleOverride;

	// LockedCells → 월드 좌표 변환
	const TArray<FGridPoint>& LockedCells = BossMgr->GetCurrentTurnLockedCells();
	TArray<FVector> CellLocations;
	for (const FGridPoint& Cell : LockedCells)
	{
		AGridActor* GridActor = GridMgr->GetGridActor(Cell);
		if (!IsValid(GridActor)) continue;

		FVector2D XY = GridActor->GetGridWorldXY();
		CellLocations.Add(FVector(XY.X, XY.Y, GridActor->GetActorLocation().Z));
	}

	// AnchorCell 월드 좌표 (LockedCells 첫 번째 또는 패턴 Spec의 AnchorCell)
	FVector AnchorLocation = FVector::ZeroVector;
	if (PatternData.PatternSpec.AnchorMode == EAreaAnchor::UseAnchorCell)
	{
		AGridActor* AnchorGrid = GridMgr->GetGridActor(PatternData.PatternSpec.AnchorCell);
		if (IsValid(AnchorGrid))
		{
			FVector2D XY = AnchorGrid->GetGridWorldXY();
			AnchorLocation = FVector(XY.X, XY.Y, AnchorGrid->GetActorLocation().Z);
		}
	}
	else if (CellLocations.Num() > 0)
	{
		// AnchorCell 모드가 아니어도 AnchorCell 타겟으로 설정했을 경우 첫 셀 사용
		AnchorLocation = CellLocations[0];
	}

	ActingMgr->PlayBossVFX(Effect, PatternData.PatternEffectTarget, Scale, CellLocations, AnchorLocation);
}
