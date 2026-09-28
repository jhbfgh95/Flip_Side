#include "AnimNotify_BossVFX.h"
#include "BossManagerSubsystem.h"
#include "BattleLevelActingWSubsystem.h"
#include "GridManagerSubsystem.h"
#include "GridActor.h"
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

	const int32 PatternIndex = BossMgr->GetCurrentPhasePatternIndex();
	UBossPatternBase* Pattern = BossMgr->GetCurrentPhasePattern();


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

	const FVector Scale = ScaleOverride.IsZero() ? PatternData.PatternScale : ScaleOverride;

	const TArray<FGridPoint>& LockedCells = BossMgr->GetCurrentPhaseLockedCells();


	TArray<FVector> CellLocations;
	for (const FGridPoint& Cell : LockedCells)
	{
		AGridActor* GridActor = GridMgr->GetGridActor(Cell);
		if (!IsValid(GridActor)) continue;

		CellLocations.Add(GridActor->GetActorLocation());
	}

	// AnchorCell은 런타임에 랜덤 결정되므로 LockedCells 중심을 사용
	FVector AnchorLocation = FVector::ZeroVector;
	if (CellLocations.Num() > 0)
	{
		for (const FVector& Loc : CellLocations)
			AnchorLocation += Loc;
		AnchorLocation /= CellLocations.Num();
	}
	else if (CellLocations.Num() > 0)
	{
		AnchorLocation = CellLocations[0];
	}


	ActingMgr->PlayBossVFX(Effect, PatternData.PatternEffectTarget, Scale, CellLocations, AnchorLocation);
}
