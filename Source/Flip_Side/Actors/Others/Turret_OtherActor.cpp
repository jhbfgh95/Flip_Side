#include "Actors/Others/Turret_OtherActor.h"
#include "AttackAreaTypes.h"
#include "GridTypes.h"
#include "BossActor.h"
#include "GridManagerSubsystem.h"

void ATurret_OtherActor::SetTurretSpawnGrid(FGridPoint targetGrid)
{
    TurretSpec.Pattern = EAttackAreaPattern::RectFromCell; 
    TurretSpec.AnchorCell = targetGrid;
    TurretSpec.Side = EAreaSide::Up;
    TurretSpec.AnchorMode = EAreaAnchor::UseAnchorCell;
    TurretSpec.ParamA = TurretRange.GridX;
    TurretSpec.ParamB = TurretRange.GridY;
    

    GridManager = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
}

void ATurret_OtherActor::OnHover_Implementation()
{
    if(GridManager)
    {
        TArray<FGridPoint> OutCells;
        FObjectOnGridInfo Info;

        GridManager->PreviewHoveredCoinRange(TurretSpec.AnchorCell, TurretSpec, TurretRange);
        GridManager->GetObjectsAtRange(TurretSpec, TurretRange, OutCells, Info);
        if(Info.Boss)
        {
            CachedBoss = Info.Boss;
            CachedBoss->DisPlayOutline();
        }
    }
}

void ATurret_OtherActor::OnUnhover_Implementation()
{
    if(GridManager)
    {
        GridManager->ResetBattleCoinPreview();
        if(CachedBoss)
        {
            CachedBoss->UnDisPlayOutline();
        }
    }
}

void ATurret_OtherActor::OnClicked_Implementation()
{
    if(GridManager)
    {
        if(CachedBoss && !bIsActed)
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