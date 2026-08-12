#include "BattleLeverActor.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
#include "FlipSide_Enum.h"

void ABattleLeverActor::ExecuteLeverLogic()
{
    UWorld* World = GetWorld();
    if (!World) return;

    auto* BattleSub = World->GetSubsystem<UBattleManagerWSubsystem>();
    
    if (!BattleSub) return;

    bCanAnimate = BattleSub->StartBattleFromLever(ABattleLeverActor::GetLeverAnimationTotalTime());
    
}

void ABattleLeverActor::OnClicked_Implementation()
{
    ExecuteLeverLogic();
    if(bCanAnimate)
    {
        if (USoundManagerWSubsystem* SoundManager = GetWorld()->GetSubsystem<USoundManagerWSubsystem>())
        {
            SoundManager->PlayLeverPullSound();
        }
        Super::PlayLeverAnimation();
    }
}

void ABattleLeverActor::OnHover_Implementation()
{
    LeverHoverOutline();
}

void ABattleLeverActor::OnUnhover_Implementation()
{
    LeverUnHoverOutline();
}
