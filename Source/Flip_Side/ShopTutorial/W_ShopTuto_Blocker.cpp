// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_ShopTuto_Blocker.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


void UW_ShopTuto_Blocker::NativeConstruct()
{
    Super::NativeConstruct();
    TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    SetVisibility(ESlateVisibility::Visible);

    for(int i =0; i< UnBlockEventOrder.Num();i++)
        TutorialSubsystem->SetTutorialEvent(UnBlockEventOrder[i],this);
}

void UW_ShopTuto_Blocker::ExecuteTutorialEvent_Implementation()
{
    UE_LOG(LogTemp,Warning, TEXT("막음"));
    SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopTuto_Blocker::FinishTutorialEvent_Implementation()
{
    SetVisibility(ESlateVisibility::Visible);
}
