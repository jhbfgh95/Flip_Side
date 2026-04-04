// Fill out your copyright notice in the Description page of Project Settings.

#include "BattlePlayerController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "BattlePlayerPawn_FlipSide.h"
#include "BattleArea.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "UseableItemActor.h"
#include "LeverActor.h"
#include "BattleManagerWSubsystem.h"
#include "BattleHoverInterface.h"
#include "BattleClickInterface.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"

ABattlePlayerController_FlipSide::ABattlePlayerController_FlipSide()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ABattlePlayerController_FlipSide::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(InputContext, 0);
    }

    InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ABattlePlayerController_FlipSide::OnLeftClick);
    InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ABattlePlayerController_FlipSide::OnRightClick);
}

void ABattlePlayerController_FlipSide::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    CheckMouseHover();
}

void ABattlePlayerController_FlipSide::BeginPlay()
{
    Super::BeginPlay();

    if (ControlledPawn)
    {
        // 디폴트 카메라 시점
        DefaultCameraLocation = FVector(-1820.0f, -990.0f, 2220.0f);
        DefaultCameraRotation = FRotator(-35.0f, 0.0f, 0.0f);
        DefaultCameraArmLength = 0.0f;
    }

    if (UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>())
    {
        BattleManager->OnTurnChanged.AddDynamic(this, &ABattlePlayerController_FlipSide::OnTurnChanged);
    }
}

void ABattlePlayerController_FlipSide::ReturnToDefaultCamera()
{
    if (ControlledPawn)
    {
        ControlledPawn->MoveCameraToArea(DefaultCameraLocation, DefaultCameraRotation, DefaultCameraArmLength);
    }
}

void ABattlePlayerController_FlipSide::OnLeftClick()
{
    FHitResult Hit;

    if (GetHitResultUnderCursor(ECC_Camera, true, Hit)) 
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor)
        {
            // 인터페이스 클릭 처리
            if (IBattleClickInterface* Clickable = Cast<IBattleClickInterface>(HitActor))
            {
                Clickable->Execute_OnClicked(HitActor);
                return;
            }
        }
    }

    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        // CurrentHoveredArea는 CheckMouseHover에서 실시간으로 업데이트됨
        if (CurrentHoveredArea && ControlledPawn)
        {
            ControlledPawn->MoveCameraToArea(
                CurrentHoveredArea->TargetLocation,
                CurrentHoveredArea->TargetRotation,
                CurrentHoveredArea->TargetArmLength);
            return;
        }
    }

    // 3. 허공 클릭 시 기본 시점 복귀
    ReturnToDefaultCamera();
}

void ABattlePlayerController_FlipSide::CheckMouseHover()
{
    FHitResult Hit;
    AActor* CurrentActor = nullptr;

    if (GetHitResultUnderCursor(ECC_Camera, true, Hit))
    {
        CurrentActor = Hit.GetActor();
    }

    if (LastHoveredActor != CurrentActor)
    {
        if (LastHoveredActor)
        {
            if (IBattleHoverInterface* PrevHover = Cast<IBattleHoverInterface>(LastHoveredActor))
            {
                PrevHover->Execute_OnUnhover(LastHoveredActor);
            }
        }

        if (CurrentActor)
        {
            if (IBattleHoverInterface* NewHover = Cast<IBattleHoverInterface>(CurrentActor))
            {
                NewHover->Execute_OnHover(CurrentActor);
            }
        }
        LastHoveredActor = CurrentActor;
    }

    // B. 기존 구역(Area) 하이라이트 체크 (기존 로직 유지)
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        ABattleArea* TargetArea = Cast<ABattleArea>(Hit.GetActor());
        if (CurrentHoveredArea != TargetArea)
        {
            if (CurrentHoveredArea) CurrentHoveredArea->SetHighlight(false);
            CurrentHoveredArea = TargetArea;
            if (CurrentHoveredArea) CurrentHoveredArea->SetHighlight(true);
        }
    }
    else
    {
        if (CurrentHoveredArea)
        {
            CurrentHoveredArea->SetHighlight(false);
            CurrentHoveredArea = nullptr;
        }
    }
}

// 우클릭: 디폴트 카메라 시점으로 복귀
void ABattlePlayerController_FlipSide::OnRightClick()
{
    UBattleManagerWSubsystem *BattleSub = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    if (BattleSub && BattleSub->GetCurrentTurn() == ETurnState::CoinSelectTurn)
    {
        /*
        BattleSub->HandleItemCanceled();
        BattleSub->HandleGridCanceled();
        */
    }

    ReturnToDefaultCamera();
}

void ABattlePlayerController_FlipSide::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<ABattlePlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);
}

void ABattlePlayerController_FlipSide::OnTurnChanged(ETurnState NewTurn)
{
    if (!ControlledPawn) return;

    if (NewTurn == ETurnState::CoinSelectTurn)
    {
        GetWorldTimerManager().SetTimer(CoinSelectCameraDelayHandle, [this]()
        {
            if (ControlledPawn)
                ControlledPawn->MoveCameraToArea(CoinSelectCameraLocation, CoinSelectCameraRotation, CoinSelectCameraArmLength);
        }, CoinSelectCameraDelay, false);
    }
    else if (NewTurn == ETurnState::CoinReadyTurn)
    {
        ControlledPawn->MoveCameraToArea(DefaultCameraLocation, DefaultCameraRotation, DefaultCameraArmLength);
    }
}