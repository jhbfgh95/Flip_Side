// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "W_ShopWidget.h"
#include "ShopPlayerPawn_FlipSide.h"
#include "UI/W_CoinCreateWidget.h"
#include "Player/GameMode_Shop.h"
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
            coinCreateWidget->SetVisibility(ESlateVisibility::Collapsed);
        }

    }

    ShopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
    if(ShopGameMode)
    {
        ShopGameMode->OnCoinCreateMode.AddDynamic(this, &AShopController_FlipSide::SetCoinCreateUI);
        ShopGameMode->OnShopMainMode.AddDynamic(this, &AShopController_FlipSide::SetMainModeUI);
        ShopGameMode->OnCoinManageMode.AddDynamic(this, &AShopController_FlipSide::SetCoinManageModeUI);
    }

    if(ShopMainWigetClass)
    {
        ShopMainWiget = CreateWidget<UUserWidget>(this, ShopMainWigetClass);
        if(ShopMainWiget)
        {
            ShopMainWiget->AddToViewport(0);
            coinCreateWidget->SetVisibility(ESlateVisibility::Collapsed);

            AddOpenWidgetList(ShopMainWiget);
            ViewWidgetList();
        }
    }

    if(CoinManageWidgetClass)
    {
        CoinManageWidget = CreateWidget<UUserWidget>(this, CoinManageWidgetClass);
        if(ShopMainWiget)
        {
            CoinManageWidget->AddToViewport(0);
            CoinManageWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

//입력 처리
void AShopController_FlipSide::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
}
//폰하고 연결
void AShopController_FlipSide::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<AShopPlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);
}

//위젯 리스트에 넣고 보이게함
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

//현재 보여지고 있는 위젯들을 다 안보이게 설정
void AShopController_FlipSide::HideWidgetList()
{
    if(0<openWidgetList.Num())
    {
        for(int i =0; i<openWidgetList.Num();i++)
        {
            openWidgetList[i]->SetVisibility(ESlateVisibility::Collapsed);
        }
        openWidgetList.Empty();
    }
}

//보이는 위젯에 추가
void AShopController_FlipSide::AddOpenWidgetList(UUserWidget* AddWidget)
{
    openWidgetList.Add(AddWidget);
}


//코인 제작 UI
void AShopController_FlipSide::SetCoinCreateUI()
{
    if(coinCreateWidget)
    {
        HideWidgetList();
        AddOpenWidgetList(coinCreateWidget);
        ViewWidgetList();
    }
}

//메인모드 UI
void AShopController_FlipSide::SetMainModeUI()
{
    HideWidgetList();
}


void AShopController_FlipSide::SetCoinManageModeUI()
{
    if(CoinManageWidget)
    {
        HideWidgetList();
        AddOpenWidgetList(CoinManageWidget);
        ViewWidgetList();
    }
}

UW_CoinCreateWidget* AShopController_FlipSide::GetCoinCreateWidget()
{
    return coinCreateWidget;
}

