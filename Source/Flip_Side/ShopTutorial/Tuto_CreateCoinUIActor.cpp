// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_CreateCoinUIActor.h"
#include "Subsystem/ShopTutorialWSubsystem.h"



void ATuto_CreateCoinUIActor::BeginPlay()
{
    Super::BeginPlay();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}


void ATuto_CreateCoinUIActor::InteractLeftClick_Implementation()
{
    ClickCoin();
    if(IsExecuteNextOrder)
        TutorialSubsytem->ExecuteEvents(NextOrder);
}