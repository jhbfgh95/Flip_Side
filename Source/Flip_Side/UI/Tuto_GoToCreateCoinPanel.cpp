// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tuto_GoToCreateCoinPanel.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


void ATuto_GoToCreateCoinPanel::BeginPlay()
{
    Super::BeginPlay();
    
    TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}

	

void ATuto_GoToCreateCoinPanel::InteractLeftClick_Implementation()
{
    ChangeCreateCoinMode();
    TutorialSubsystem->ExecuteEvents(ExecuteOrder);
}