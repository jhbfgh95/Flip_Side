// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockCard/ShopUnlockCardLever.h"
#include "Subsystem/ShopLevel/ShopUnlockCardWSubsystem.h"
#include "Components/BoxComponent.h"

AShopUnlockCardLever::AShopUnlockCardLever()
{
    InteractBox = CreateDefaultSubobject<UBoxComponent>("InteracBox");
    InteractBox->SetupAttachment(RootComponent);
}
void AShopUnlockCardLever::BeginPlay()
{
    Super::BeginPlay();
    UnlockCardSubsystem = GetWorld()->GetSubsystem<UShopUnlockCardWSubsystem>();
}

void AShopUnlockCardLever::ExecuteLeverLogic() 
{
    UnlockCardSubsystem->UnlockCard();
}

void AShopUnlockCardLever::InteractLeftClick_Implementation()
{
	OnLeverInteracted();
}