#include "CoinSlotActor.h"
#include "CoinActor.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/FlipSideSoundUtils.h"

ACoinSlotActor::ACoinSlotActor()
{

}

void ACoinSlotActor::BeginPlay()
{
	Super::BeginPlay();

    if(AllowcatedCoins.IsEmpty())
    {
       Destroy();
    }
}

void ACoinSlotActor::OnHover_Implementation()
{
    OnCoinSlotHovered.Broadcast(this);
    SlotHoverOutline();
    
    for (ACoinActor* Coin : AllowcatedCoins)
    {
        if (Coin) Coin->CoinHoverOutline();
    }
}

void ACoinSlotActor::OnUnhover_Implementation()
{
    OnCoinSlotUnHovered.Broadcast();
    SlotUnHoverOutline();

    for (ACoinActor* Coin : AllowcatedCoins)
    {
        if (Coin) Coin->CoinUnHoverOutline();
    }
}

void ACoinSlotActor::OnClicked_Implementation()
{
    if (bClickCooldown) return;

    for (ACoinActor* Coin : AllowcatedCoins)
    {
        if (Coin && !Coin->GetCoinIsReady() && Coin->SameTypeIndex == 0)
        {
            bClickCooldown = true;
            GetWorldTimerManager().SetTimer(ClickCooldownTimer, [this]()
            {
                bClickCooldown = false;
            }, 0.3f, false);

            FFlipSideSoundUtils::PlayDefaultClickSound(this);
            OnCoinSlotClicked.Broadcast(Coin);
            return;
        }
    }
}
