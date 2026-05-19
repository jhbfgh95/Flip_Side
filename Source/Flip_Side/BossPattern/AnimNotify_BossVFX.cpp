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

	UE_LOG(LogTemp, Warning, TEXT("[BossVFX] Notify fired. BossMgr=%d ActingMgr=%d GridMgr=%d"),
		BossMgr != nullptr, ActingMgr != nullptr, GridMgr != nullptr);

	if (!BossMgr || !ActingMgr || !GridMgr) return;

	const int32 PatternIndex = BossMgr->GetCurrentTurnPatternIndex();
	UBossPatternBase* Pattern = BossMgr->GetCurrentTurnPattern();

	UE_LOG(LogTemp, Warning, TEXT("[BossVFX] PatternIndex=%d, Pattern=%d"), PatternIndex, Pattern != nullptr);

	if (!Pattern || PatternIndex == INDEX_NONE) return;

	FBossPatternBattleData PatternData;
	ABossActor* Boss = BossMgr->GetCurrentBoss();
	if (!Boss || !Boss->GetPatternData(PatternIndex, PatternData)) return;

	UE_LOG(LogTemp, Warning, TEXT("[BossVFX] PatternEffect isNull=%d, EffectTarget=%d"),
		PatternData.PatternEffect.IsNull(), (int32)PatternData.PatternEffectTarget);

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

	UE_LOG(LogTemp, Warning, TEXT("[BossVFX] Effect=%d"), Effect != nullptr);

	if (!Effect) return;

	const FVector Scale = ScaleOverride.IsZero() ? PatternData.PatternScale : ScaleOverride;

	const TArray<FGridPoint>& LockedCells = BossMgr->GetCurrentTurnLockedCells();

	UE_LOG(LogTemp, Warning, TEXT("[BossVFX] LockedCells count=%d"), LockedCells.Num());

	TArray<FVector> CellLocations;
	for (const FGridPoint& Cell : LockedCells)
	{
		AGridActor* GridActor = GridMgr->GetGridActor(Cell);
		if (!IsValid(GridActor)) continue;

		CellLocations.Add(GridActor->GetActorLocation());
	}

	if (CellLocations.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossVFX] First CellLocation=%s"), *CellLocations[0].ToString());
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

	UE_LOG(LogTemp, Warning, TEXT("[BossVFX] Calling PlayBossVFX. CellLocations=%d, AnchorLocation=%s"),
		CellLocations.Num(), *AnchorLocation.ToString());

	ActingMgr->PlayBossVFX(Effect, PatternData.PatternEffectTarget, Scale, CellLocations, AnchorLocation);
}
