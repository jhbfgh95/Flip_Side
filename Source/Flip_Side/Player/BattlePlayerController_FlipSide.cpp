// Fill out your copyright notice in the Description page of Project Settings.

#include "BattlePlayerController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "BattlePlayerPawn_FlipSide.h"
#include "BattleArea.h"
#include "CoinActor.h"
#include "LeverActor.h"
#include "BattleManagerWSubsystem.h"
#include "Subsystem/CoinManagementWSubsystem.h"

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
        DefaultCameraLocation = FVector(-1656.599739f, -1043.456394f, 2234.021618f);
        DefaultCameraRotation = FRotator(-35.0f, -0.20f, 0.0f);
        DefaultCameraArmLength = 0.0f;
    }
}

void ABattlePlayerController_FlipSide::ReturnToDefaultCamera()
{
    if (ControlledPawn)
    {
        ControlledPawn->MoveCameraToArea(DefaultCameraLocation, DefaultCameraRotation, DefaultCameraArmLength);
    }
}

// 좌클릭: 선택, 카메라 이동
void ABattlePlayerController_FlipSide::OnLeftClick()
{
    UBattleManagerWSubsystem *BattleSubsystem = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    FHitResult Hit;

    if (GetHitResultUnderCursor(ECC_Camera, false, Hit))
    {
        ACoinActor *ClickedCoin = Cast<ACoinActor>(Hit.GetActor());

        if (ClickedCoin)
        {
            if (BattleSubsystem && BattleSubsystem->GetCurrentTurn() == ETurnState::CoinReadyTurn)
            {
                UCoinManagementWSubsystem *CoinSubsystem = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
                if (!CoinSubsystem)
                    return;

                int32 TargetID = ClickedCoin->GetCoinID();

                if (CoinSubsystem->IsCoinIdInBattleReady(TargetID))
                {
                    // 서랍 코인 취소 실행
                    CoinSubsystem->RemoveBattleReadyCoins(ClickedCoin);
                    return;
                }
                else if (ClickedCoin->GetSameTypeIndex() == 0)
                {
                    // 슬롯 코인->서랍
                    CoinSubsystem->AddBattleReadyCoins(ClickedCoin);
                    return;
                }
            }
        }
        // 레버 클릭
        else if (ALeverActor *ClickedLever = Cast<ALeverActor>(Hit.GetActor()))
        {
            ClickedLever->OnLeverInteracted();
            return;
        }
    }

    // 영역 판단
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        if (CurrentHoveredArea && ControlledPawn)
        {
            ControlledPawn->MoveCameraToArea(
                CurrentHoveredArea->TargetLocation,
                CurrentHoveredArea->TargetRotation,
                CurrentHoveredArea->TargetArmLength);
            return;
        }
    }
    // 빈 공간 클릭 시 디폴트 카메라 시점으로 복귀
    ReturnToDefaultCamera();
}

// 우클릭: 디폴트 카메라 시점으로 복귀
void ABattlePlayerController_FlipSide::OnRightClick()
{
    ReturnToDefaultCamera();
}

void ABattlePlayerController_FlipSide::CheckMouseHover()
{
    FHitResult Hit;
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        AActor *HitActor = Hit.GetActor();

        ABattleArea *TargetArea = Cast<ABattleArea>(HitActor);

        // 이전에 호버링하던 곳과 지금 마우스 아래 있는 곳이 다를 때
        if (CurrentHoveredArea != TargetArea)
        {
            if (CurrentHoveredArea)
            {
                CurrentHoveredArea->SetHighlight(false);
            }

            CurrentHoveredArea = TargetArea;

            if (CurrentHoveredArea)
            {
                CurrentHoveredArea->SetHighlight(true);
            }
        }
    }
    else
    {
        // 마우스가 허공을 보거나 아무 영역도 아닐 때
        if (CurrentHoveredArea)
        {
            CurrentHoveredArea->SetHighlight(false);
            CurrentHoveredArea = nullptr;
        }
    }
}

void ABattlePlayerController_FlipSide::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<ABattlePlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);
}