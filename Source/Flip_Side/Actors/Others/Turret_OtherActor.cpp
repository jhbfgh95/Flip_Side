#include "Actors/Others/Turret_OtherActor.h"
#include "AttackAreaTypes.h"
#include "GridTypes.h"
#include "BossActor.h"
#include "GridManagerSubsystem.h"

void ATurret_OtherActor::InitializeTurret(FGridPoint TargetGrid, const FAttackAreaSpec& AttackAreaSpec)
{
    TurretSpawnGrid = TargetGrid;
    TurretSpec = AttackAreaSpec;
    TurretSpec.AnchorCell = FGridPoint{0, 0};
    TurretSpec.AnchorMode = EAreaAnchor::UseAnchorCell;

	UWorld* World = GetWorld();
	GridManager = IsValid(World) ? World->GetSubsystem<UGridManagerSubsystem>() : nullptr;
}

void ATurret_OtherActor::OnHover_Implementation()
{
    if(GridManager)
    {
        TArray<FGridPoint> OutCells;
        ABossActor* Boss = nullptr;

        GridManager->PreviewHoveredCoinRange(TurretSpawnGrid, TurretSpec, TurretSpawnGrid);
        GridManager->CollectAttackRangeTargets(TurretSpawnGrid, TurretSpec, OutCells, Boss);
        if(IsValid(Boss))
        {
            CachedBoss = Boss;
            CachedBoss->DisPlayOutline();
        }
    }
}

void ATurret_OtherActor::OnUnhover_Implementation()
{
    if(GridManager)
    {
        GridManager->ResetBattleCoinPreview();
		if(IsValid(CachedBoss))
		{
			CachedBoss->UnDisPlayOutline();
		}
		CachedBoss = nullptr;
    }
}

void ATurret_OtherActor::OnClicked_Implementation()
{
    if(TryBroadcastOtherClicked())
    {
        return;
    }

    if(GridManager)
    {
		if(IsValid(CachedBoss) && !bIsActed)
        {
            CachedBoss->ApplyDamage(AttackPoint, this);
            bIsActed = true;
        }
        else
        {
            //안 들어옴 피드백 + 공격함 피드백
            return;
        }
    }
}
