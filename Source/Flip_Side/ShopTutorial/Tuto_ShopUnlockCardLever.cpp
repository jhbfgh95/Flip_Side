// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_ShopUnlockCardLever.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


void ATuto_ShopUnlockCardLever::BeginPlay()
{
    Super::BeginPlay();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}

void ATuto_ShopUnlockCardLever::ExecuteLeverLogic()
{
    Super::ExecuteLeverLogic();
    
    if(IsExecuteNextOrder)
        TutorialSubsytem->ExecuteEvents(NextOrder);
}
