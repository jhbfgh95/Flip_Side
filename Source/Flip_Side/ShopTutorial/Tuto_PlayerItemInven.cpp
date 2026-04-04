// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_PlayerItemInven.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


void ATuto_PlayerItemInven::BeginPlay()
{
    Super::BeginPlay();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}

void ATuto_PlayerItemInven::ExecuteNextTutorial()
{
    
    if(IsExecuteNextOrder)
        TutorialSubsytem->ExecuteEvents(NextOrder);
}
	
void ATuto_PlayerItemInven::InteractLeftClick_Implementation()
{
    ActiveInven();
    ExecuteNextTutorial();
}
