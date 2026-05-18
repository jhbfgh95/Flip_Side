// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_ShopCreateCoin.h"
#include "Subsystem/ShopTutorialWSubsystem.h"



void ATuto_ShopCreateCoin::BeginPlay()
{
    Super::BeginPlay();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}


void ATuto_ShopCreateCoin::InteractLeftClick_Implementation()
{
    ClickCoin();
    TutoSubsystem->ExecuteEvents(ExcuteOrder);
}
