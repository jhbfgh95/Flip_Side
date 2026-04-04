// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_CoinSlotLockPanel.h"
#include "Subsystem/ShopTutorialWSubsystem.h"

void ATuto_CoinSlotLockPanel::BeginPlay()
{
    Super::BeginPlay();
    TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();

}

void ATuto_CoinSlotLockPanel::InteractLeftClick_Implementation()
{
    UnlockCoinSlot();
    if(TutorialSubsystem)
        TutorialSubsystem->ExecuteEvents(ExecuteOrder);
}