// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_UnlockWeaponClassLever.h"
#include "Subsystem/ShopTutorialWSubsystem.h"



void ATuto_UnlockWeaponClassLever::BeginPlay()
{
    Super::BeginPlay();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}

void ATuto_UnlockWeaponClassLever::ExecuteLeverLogic()
{
    Super::ExecuteLeverLogic();
    
    if(IsExecuteNextOrder)
        TutorialSubsytem->ExecuteEvents(NextOrder);
}

