// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_ShopPlayerCard.h"
#include "Subsystem/ShopTutorialWSubsystem.h"

void UW_Tuto_ShopPlayerCard::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}


void UW_Tuto_ShopPlayerCard::RClickCard()
{
    Super::RClickCard();
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}

