// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_ShopCardWidget.h"
#include "Subsystem/ShopTutorialWSubsystem.h"

void UW_Tuto_ShopCardWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}

void UW_Tuto_ShopCardWidget::LeftClickCard()
{
    Super::LeftClickCard();
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}