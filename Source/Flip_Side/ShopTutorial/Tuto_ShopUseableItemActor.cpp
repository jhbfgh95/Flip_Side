// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_ShopUseableItemActor.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


void ATuto_ShopUseableItemActor::BeginPlay()
{
    Super::BeginPlay();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}

void ATuto_ShopUseableItemActor::ExecuteNextTutorial()
{
    
    if(IsExecuteNextOrder)
        TutorialSubsytem->ExecuteEvents(NextOrder);
}
	
void ATuto_ShopUseableItemActor::InteractLeftClick_Implementation()
{
    ClickItem();
    ExecuteNextTutorial();
}