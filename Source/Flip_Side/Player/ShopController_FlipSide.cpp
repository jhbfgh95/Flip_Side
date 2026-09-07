// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "W_ShopWidget.h"
#include "ShopPlayerPawn_FlipSide.h"

#include "Subsystem/MoneyGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"

#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"

#include "UI/ShopItem/ShopItemPresenter.h"
#include "UI/ShopItem/ShopItemUIActor.h"
#include "UI/ShopCoinManage/ShopCoinUIActor.h"
#include "UI/ShopCard/ShopCardPresenter.h"
#include "UI/ShopCoinManage/ShopCoinPresenter.h"
#include "UI/ShopUnlockWeapon/ShopUnlockWeaponUIActor.h"
#include "UI/ShopUnlockWeapon/UnlockWeaponPresenter.h"
#include "UI/ShopPageChangePresenter.h"
#include "UI/ShopUISelectRegistry.h"

#include "Interface/ShopMouseInterface.h"
#include "UI/W_ShopWidgetContainer.h"
#include "Kismet/GameplayStatics.h"
AShopController_FlipSide::AShopController_FlipSide()
{
    bShowMouseCursor = true;
}

void AShopController_FlipSide::BeginPlay()
{
    Super::BeginPlay();
    
    this->bShowMouseCursor = true;
    this->bEnableMouseOverEvents = true;
    CanClick = true;
////////////////////////////
    /*서브시스템*/
    UDataManagerSubsystem* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    UShopItemWSubsystem* ItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    UUnlockGISubsystem* UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    UShopCardWSubsystem* CardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    UShopCoinWSubsystem* CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    UShopUnlockWeaponWSubsystem* UnlockWeaponWSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
    ShopWidgetContainer = Cast<UW_ShopWidgetContainer>(CreateWidget<UUserWidget>(this, ShopWidgetContainerClass));
    
    /*메인 및 각 UI 액터들 탐색*/
    ShopUISelectRegistry = Cast<AShopUISelectRegistry>(
        UGameplayStatics::GetActorOfClass(this, AShopUISelectRegistry::StaticClass()));

    if (!ensure(ShopWidgetContainer)){return;}

    // 레벨에 배치된 상점 아이템 UI 액터를 찾아 프레젠터에 전달한다.
    ShopItemUIActor = Cast<AShopItemUIActor>(
        UGameplayStatics::GetActorOfClass(this, AShopItemUIActor::StaticClass()));

    if (!ensure(IsValid(ShopItemUIActor)))return;

	ShopCoinUIActor = Cast<AShopCoinUIActor>(
		UGameplayStatics::GetActorOfClass(this, AShopCoinUIActor::StaticClass()));

	if (!ensure(IsValid(ShopCoinUIActor))){return;}

	ShopUnlockWeaponUIActor = Cast<AShopUnlockWeaponUIActor>(
		UGameplayStatics::GetActorOfClass(this, AShopUnlockWeaponUIActor::StaticClass()));
	if (!ensure(IsValid(ShopUnlockWeaponUIActor))){return;}

    /*Presenter 초기화*/
    ItemPresenter = NewObject<UShopItemPresenter>(this);
    ItemPresenter->InitPresenter(ShopWidgetContainer->GetShopItemWidget(), ItemSubsystem, DataManager, ShopItemUIActor);
    
    CardPresenter = NewObject<UShopCardPresenter>();
    CardPresenter->InitPresenter(ShopWidgetContainer->GetShopCardWidget(), CardSubsystem, DataManager, UnlockSubsystem);

    CoinPresenter = NewObject<UShopCoinPresenter>(this);
    CoinPresenter->InitPresenter(ShopWidgetContainer->GetShopCoinWidget(), CoinSubsystem, DataManager, UnlockSubsystem, ShopCoinUIActor);

    UnlockWeaponPresenter = NewObject<UUnlockWeaponPresenter>(this);
    UnlockWeaponPresenter->InitPresenter(ShopWidgetContainer->GetShopUnlockWeaponWidget(),UnlockWeaponWSubsystem, DataManager, UnlockSubsystem, ShopUnlockWeaponUIActor);

    TryInitPageChangePresenter();
    
    ShopWidgetContainer->AddToViewport();
    UMoneyGISubsystem* MoneySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMoneyGISubsystem>();
    if(MoneySubsystem)
        MoneySubsystem->UpdateMoneyDisplayWidget();

	if (IsValid(ESCWidgetClass))
    {
        ESCWidget = CreateWidget<UUserWidget>(this, ESCWidgetClass);
	    ESCWidget->AddToViewport();
	    ESCWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    
    //InitWidget(BlockWidgetClass,BlockWidget,20);
    //InitWidget(ShopMainWidgetClass,ShopMainWidget,0);
    //InitWidget(ShopModeWidgetClass,ShopModeWidget,0);
    
    //AddOpenWidgetList(ShopWidgetContainer);

    //ViewWidgetList();

    //SetLockMouse(false);

}


void AShopController_FlipSide::InitWidget(TSubclassOf<UUserWidget> WidgetClass, UUserWidget*& Widget,int32 ZOrder)
{
    if(WidgetClass)
    {
        Widget = CreateWidget<UUserWidget>(this, WidgetClass);
        if(Widget)
        {
            Widget->AddToViewport(ZOrder);
            Widget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

//입력 처리
void AShopController_FlipSide::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(InputContext, 0);
    }

    InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AShopController_FlipSide::OnLeftClick);
	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AShopController_FlipSide::OpenESCWidget);
    InputComponent->BindKey(EKeys::CapsLock, IE_Pressed, this, &AShopController_FlipSide::OpenESCWidget);
    //InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ABattlePlayerController_FlipSide::OnRightClick);
}

void AShopController_FlipSide::OpenESCWidget()
{
	if (!IsValid(ESCWidget)){return;}
    if(IsESCWidgetOpen)
    {
        SetLockMouse(false);
        IsESCWidgetOpen = false;
        ESCWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        SetLockMouse(true);
        IsESCWidgetOpen = true;
        ESCWidget->SetVisibility(ESlateVisibility::Visible);
    }
	
}

//폰하고 연결
void AShopController_FlipSide::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<AShopPlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);

	TryInitPageChangePresenter();
    UE_LOG(LogTemp, Warning, TEXT("폰에서 시작 "));
}

void AShopController_FlipSide::TryInitPageChangePresenter()
{
    if (IsValid(PageChangePresenter) ||
        !IsValid(ShopWidgetContainer) || !IsValid(ControlledPawn) || !IsValid(ShopUISelectRegistry))
    {
        return;
    }

    PageChangePresenter = NewObject<UShopPageChangePresenter>(this);
    PageChangePresenter->InitPresenter(ShopUISelectRegistry, ShopWidgetContainer, ControlledPawn);
}


void AShopController_FlipSide::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    CheckMouseHover();
}
//위젯 리스트에 넣고 보이게함
void AShopController_FlipSide::ViewWidgetList()
{
    if(0<openWidgetList.Num())
    {
        for(int i =0; i<openWidgetList.Num();i++)
        {
            if(openWidgetList[i])
            {
                openWidgetList[i]->SetVisibility(ESlateVisibility::Visible);
            }
            
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
            openWidgetList[i]->SetVisibility(ESlateVisibility::Hidden);
        }
        openWidgetList.Empty();
    }
}

//보이는 위젯에 추가
void AShopController_FlipSide::AddOpenWidgetList(UUserWidget* AddWidget)
{
    openWidgetList.Add(AddWidget);
}


//메인모드 UI
void AShopController_FlipSide::SetShopMainModeWidget()
{
    HideWidgetList();
    AddOpenWidgetList(ShopMainWidget);
    ViewWidgetList();
    ShopModeWidget->SetVisibility(ESlateVisibility::Hidden);
}


void AShopController_FlipSide::InitShopUISelectActor()
{

}




void AShopController_FlipSide::OnLeftClick()
{   
    if(CanClick)
    {
        UE_LOG(LogTemp, Warning, TEXT("클릭이벤트"));
        FHitResult Hit;

        if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
        {
            if (Hit.Component->ComponentHasTag("LClickAble") && Hit.GetActor()->Implements<UShopMouseInterface>())
            {
                IShopMouseInterface::Execute_InteractLeftClick(Hit.GetActor());
            }
        }
    }
	
}

void AShopController_FlipSide::OnRightClick()
{
    if(CanClick)
    {

    }
    //ReturnToDefaultCamera();
}

void AShopController_FlipSide::CheckMouseHover()
{
    if(!CanClick)
        return;
    FHitResult Hit;
    AActor* NewHoverActor = nullptr;
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        if (Hit.Component->ComponentHasTag("HoverAble") && Hit.GetActor()->Implements<UShopMouseInterface>())
        {
            NewHoverActor = Hit.GetActor();
        }
    }

    if (CurrentHoverActor != NewHoverActor)
    {
        if (CurrentHoverActor && CurrentHoverActor->Implements<UShopMouseInterface>())
        {
            IShopMouseInterface::Execute_InteractUnHover(CurrentHoverActor);
        }

        CurrentHoverActor = NewHoverActor;

        if (CurrentHoverActor)
        {
            IShopMouseInterface::Execute_InteractHover(CurrentHoverActor);
        }
    }
}

void AShopController_FlipSide::SetLockMouse(bool IsMouseLock)
{
    if(IsMouseLock)
    {
        //BlockWidget->SetVisibility(ESlateVisibility::Visible);
        CanClick = false;
    }
    else
    {
        //BlockWidget->SetVisibility(ESlateVisibility::Hidden);
        CanClick = true;
    }
}


