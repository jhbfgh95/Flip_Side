#include "BattleLeverActor.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h" 
#include "FlipSide_Enum.h" 

void ABattleLeverActor::ExecuteLeverLogic()
{
    UWorld* World = GetWorld();
    if (!World) return;

    auto* BattleSub = World->GetSubsystem<UBattleManagerWSubsystem>();
    
    if (!BattleSub) return;

    if(BattleSub->GetCurrentTurn() != ETurnState::BossTurn || BattleSub->GetCurrentTurn() != ETurnState::SettingTurn)
    {
        BattleSub->StartBattleFromLever();
    }
}

void ABattleLeverActor::OnClicked_Implementation()
{
    ExecuteLeverLogic();
    Super::PlayLeverAnimation();
}