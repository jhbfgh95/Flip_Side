#include "CoinSlotActor.h"
#include "CoinActor.h"
#include "Components/StaticMeshComponent.h"

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
}

void ACoinSlotActor::OnUnhover_Implementation()
{
    OnCoinSlotUnHovered.Broadcast();
}

void ACoinSlotActor::OnClicked_Implementation()
{
    for (ACoinActor* Coin : AllowcatedCoins)
    {
        if (Coin && !Coin->GetCoinIsReady() && Coin->SameTypeIndex == 0)
        {
            OnCoinSlotClicked.Broadcast(Coin);
            return;
        }
    }
}