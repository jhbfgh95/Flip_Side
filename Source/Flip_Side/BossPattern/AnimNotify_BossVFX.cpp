#include "AnimNotify_BossVFX.h"
#include "BossActor.h"
#include "Engine/World.h"
#include "BossManagerSubsystem.h"
#include "BattleLevelActingWSubsystem.h"
#include "GridManagerSubsystem.h"
#include "GridActor.h"
#include "NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_BossVFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp)) return;

	UWorld* World = MeshComp->GetWorld();
	if (!IsValid(World) || !World->IsGameWorld()) return;

	UBossManagerSubsystem* BossMgr = World->GetSubsystem<UBossManagerSubsystem>();
	UBattleLevelActingWSubsystem* ActingMgr = World->GetSubsystem<UBattleLevelActingWSubsystem>();
	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();


	if (!IsValid(BossMgr) || !IsValid(ActingMgr) || !IsValid(GridMgr) || !BossMgr->IsAttackExecuting()) return;

	const int32 PatternIndex = BossMgr->GetCurrentPhasePatternIndex();
	UBossPatternBase* Pattern = BossMgr->GetCurrentPhasePattern();


	if (!Pattern || PatternIndex == INDEX_NONE) return;

	FBossPatternBattleData PatternData;
	ABossActor* Boss = BossMgr->GetCurrentBoss();
	if (!IsValid(Boss) || MeshComp->GetOwner() != Boss || MeshComp != Boss->BossMesh || Boss->IsStunned() ||
		!Boss->GetPatternData(PatternIndex, PatternData)) return;


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


	if (!IsValid(Effect))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossNotify] BossVFX effect missing. Boss=%s PatternIndex=%d"), *GetNameSafe(Boss), PatternIndex);
		return;
	}

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
	const EBossPatternTarget Target = bOverrideEffectTarget ? EffectTarget : PatternData.PatternEffectTarget;
	if (Target == EBossPatternTarget::BossLocation)
	{
		AnchorLocation = Boss->GetSelfEffectLocation();
		ActingMgr->PlayBossVFX(Effect, EBossPatternTarget::AnchorCell, Scale, CellLocations, AnchorLocation);
		return;
	}
	ActingMgr->PlayBossVFX(Effect, Target, Scale, CellLocations, AnchorLocation);
}
