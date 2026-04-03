// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_ShopTuto_Blocker.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


void UW_ShopTuto_Blocker::NativeConstruct()
{
    Super::NativeConstruct();
    TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    SetVisibility(ESlateVisibility::Visible);

    for(int i =0; i< EventOrder.Num();i++)
        TutorialSubsystem->SetTutorialEvent(EventOrder[i],this);
}

void UW_ShopTuto_Blocker::ExecuteTutorialEvent_Implementation()
{
    SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopTuto_Blocker::FinishTutorialEvent_Implementation()
{
    SetVisibility(ESlateVisibility::Visible);
}
