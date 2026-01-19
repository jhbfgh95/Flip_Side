// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "W_ShopWidget.h"
#include "ShopPlayerPawn_FlipSide.h"
#include "UI/W_CoinCreateWidget.h"
AShopController_FlipSide::AShopController_FlipSide()
{
    bShowMouseCursor = true;
}

void AShopController_FlipSide::BeginPlay()
{
    Super::BeginPlay();
    /*
    if(ShopWigetClass)
    {
        HudShopWidget = CreateWidget<UW_ShopWidget>(this, ShopWigetClass);
        if(HudShopWidget)
        {
            HudShopWidget->AddToViewport(0);
        }
    }
*/
    if(coinCreateWigetClass)
    {
        coinCreateWidget = CreateWidget<UW_CoinCreateWidget>(this, coinCreateWigetClass);
        if(coinCreateWidget)
        {
            coinCreateWidget->AddToViewport(0);
        }
    }
}

void AShopController_FlipSide::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void AShopController_FlipSide::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<AShopPlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);
}



void AShopController_FlipSide::ViewWidgetList()
{
    if(0<openWidgetList.Num())
    {
        for(int i =0; i<openWidgetList.Num();i++)
        {
            openWidgetList[i]->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void AShopController_FlipSide::HideWidgetList()
{
    if(0<openWidgetList.Num())
    {
        for(int i =0; i<openWidgetList.Num();i++)
        {
            openWidgetList[i]->SetVisibility(ESlateVisibility::Hidden);
        }
        openWidgetList.Empty();
    }
}

void AShopController_FlipSide::AddOpenWidgetList(UUserWidget* AddWidget)
{
    openWidgetList.Add(AddWidget);
}


//코인 제작 UI
void AShopController_FlipSide::SetCoinCreateUI()
{
    if(coinCreateWidget)
    {
        coinCreateWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

UW_CoinCreateWidget* AShopController_FlipSide::GetCoinCreateWidget()
{
    return coinCreateWidget;
}

