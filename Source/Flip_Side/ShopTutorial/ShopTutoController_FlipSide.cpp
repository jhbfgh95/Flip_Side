// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/ShopTutoController_FlipSide.h"


void AShopTutoController_FlipSide::SetNonTutorialWidget()
{
    ShopMainWidgetClass = NonTutoShopMainWidgetClass;
    ShopModeWidgetClass = NonTutoShopModeWidgetClass;

    HideWidgetList();
    SetShopMainModeWidget();
    UE_LOG(LogTemp, Warning, TEXT("변신"));
}