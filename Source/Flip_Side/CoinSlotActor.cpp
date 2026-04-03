#include "CoinSlotActor.h"
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
    if(AllowcatedCoins.IsValidIndex(OutCoinNum))
    {
        OnCoinSlotClicked.Broadcast(AllowcatedCoins[OutCoinNum]);
        OutCoinNum++;
    }
    
    return;
}