// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_SelectClassLever.h"
#include "Subsystem/ShopTutorialWSubsystem.h"



void ATuto_SelectClassLever::BeginPlay()
{
    Super::BeginPlay();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    if(TutorialSubsytem)
        TutorialSubsytem->SetTutorialEvent(Order, this);
}

void ATuto_SelectClassLever::ExecuteTutorialEvent_Implementation()
{

}

void ATuto_SelectClassLever::ExecuteLeverLogic()
{
    Super::ExecuteLeverLogic();
    
    if(IsExecuteNextOrder)
        TutorialSubsytem->ExecuteEvents(NextOrder);
}
