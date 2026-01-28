// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "W_ShopWidget.h"
#include "ShopPlayerPawn_FlipSide.h"
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
    //게임모드에서 델리게이트에 등록
    ShopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());

    if(ShopGameMode)
    {
        ShopGameMode->OnCoinCreateMode.AddDynamic(this, &AShopController_FlipSide::SetCoinCreateWidget);
        ShopGameMode->OnShopMainMode.AddDynamic(this, &AShopController_FlipSide::SetShopMainModeWidget);
        ShopGameMode->OnCoinManageMode.AddDynamic(this, &AShopController_FlipSide::SetCoinManageModeWidget);
        ShopGameMode->OnShopItemMode.AddDynamic(this, &AShopController_FlipSide::SetShopItemModeWidget);
        ShopGameMode->OnSelectCardMode.AddDynamic(this, &AShopController_FlipSide::SetSelectCardModeWidget);
    }
    //코인 제작UI 초기 설정
    if(CoinCreateWigetClass)
    {
        
        CoinCreateWidget = CreateWidget<UUserWidget>(this, CoinCreateWigetClass);
        if(CoinCreateWidget)
        {
            CoinCreateWidget->AddToViewport(0);
            CoinCreateWidget->SetVisibility(ESlateVisibility::Collapsed);
        }

    }
    //상점 메인 UI 초기설정
    if(ShopMainWigetClass)
    {
        ShopMainWiget = CreateWidget<UUserWidget>(this, ShopMainWigetClass);
        if(ShopMainWiget)
        {
            ShopMainWiget->AddToViewport(0);
            CoinCreateWidget->SetVisibility(ESlateVisibility::Collapsed);

            AddOpenWidgetList(ShopMainWiget);
            ViewWidgetList();
        }
    }
    //코인 관리 UI 초기 설정
    if(CoinManageWidgetClass)
    {
        CoinManageWidget = CreateWidget<UUserWidget>(this, CoinManageWidgetClass);
        if(ShopMainWiget)
        {
            CoinManageWidget->AddToViewport(0);
            CoinManageWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    //상점 UI 클래스
    if(ShopItemWidgetClass)
    {
        ShopItemWidget = CreateWidget<UUserWidget>(this, ShopItemWidgetClass);
        if(ShopItemWidget)
        {
            ShopItemWidget->AddToViewport(0);
            ShopItemWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    //카드선택 UI 초기설정
    if(SelectCardWidgetClass)
    {
        SelectCardWidget = CreateWidget<UUserWidget>(this, SelectCardWidgetClass);
        if(SelectCardWidget)
        {
            SelectCardWidget->AddToViewport(0);
            SelectCardWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    if(ModeChangeWidgetClass)
    {
        ModeChangeWidget = CreateWidget<UUserWidget>(this, ModeChangeWidgetClass);
        if(SelectCardWidget)
        {
            ModeChangeWidget->AddToViewport(0);
            ModeChangeWidget->SetVisibility(ESlateVisibility::Collapsed);
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
void AShopController_FlipSide::SetCoinCreateWidget()
{
    HideModeChangeWidget();
    if(CoinCreateWidget)
    {
        HideWidgetList();
        AddOpenWidgetList(CoinCreateWidget);
        ViewWidgetList();
    }
}

//메인모드 UI
void AShopController_FlipSide::SetShopMainModeWidget()
{
    HideWidgetList();
    AddOpenWidgetList(ShopMainWiget);
    ViewWidgetList();

    HideModeChangeWidget();
}


void AShopController_FlipSide::SetCoinManageModeWidget()
{
    ShowModeChangeWidget();
    if(CoinManageWidget)
    {
        HideWidgetList();
        AddOpenWidgetList(CoinManageWidget);
        ViewWidgetList();
    }
}
//카드 상점UI 띄움
void AShopController_FlipSide::SetShopItemModeWidget()
{
    ShowModeChangeWidget();
    if(ShopItemWidget)
    {
        HideWidgetList();
        AddOpenWidgetList(ShopItemWidget);
        ViewWidgetList();
    }
}

//카드 선택UI 띄움
void AShopController_FlipSide::SetSelectCardModeWidget()
{
    ShowModeChangeWidget();
    if(SelectCardWidget)
    {
        
        AddOpenWidgetList(SelectCardWidget);
        ViewWidgetList();
    }
}

void AShopController_FlipSide::HideModeChangeWidget()
{
    if(ModeChangeWidget)
    {
        ModeChangeWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void AShopController_FlipSide::ShowModeChangeWidget()
{
    if(ModeChangeWidget)
    {
        ModeChangeWidget->SetVisibility(ESlateVisibility::Visible);
    }
}