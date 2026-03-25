// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_Tuto_Widget.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


void UW_Tuto_Widget::NativeConstruct()
{
    Super::NativeConstruct();
    TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    SetVisibility(ESlateVisibility::Collapsed);

    for(int i =0; i< EventOrder.Num();i++)
        TutorialSubsystem->SetTutorialEvent(EventOrder[i],this);
}

void UW_Tuto_Widget::ExecuteTutorialEvent_Implementation()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UW_Tuto_Widget::FinishTutorialEvent_Implementation()
{
    SetVisibility(ESlateVisibility::Collapsed);
}