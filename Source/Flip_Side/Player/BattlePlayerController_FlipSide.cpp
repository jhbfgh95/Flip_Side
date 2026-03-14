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
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
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

// 좌클릭
void ABattlePlayerController_FlipSide::OnLeftClick()
{
    UBattleManagerWSubsystem *BattleSub = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    UGridManagerSubsystem *GridSub = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
    if (!BattleSub || !GridSub)
        return;

    FHitResult Hit;
    ETurnState CurrentTurn = BattleSub->GetCurrentTurn();

    // 레버 클릭
    if (GetHitResultUnderCursor(ECC_Camera, false, Hit))
    {
        if (ALeverActor *ClickedLever = Cast<ALeverActor>(Hit.GetActor()))
        {
            ClickedLever->OnLeverInteracted();
            return;
        }
    }

    // CoinSelectTurn
    if (CurrentTurn == ETurnState::CoinSelectTurn)
    {
        // 액터 및 그리드 통합 체크
        if (GetHitResultUnderCursor(ECC_Camera, false, Hit))
        {
            AActor *HitActor = Hit.GetActor();

            // 아이템 클릭
            if (AUseableItemActor *Item = Cast<AUseableItemActor>(HitActor))
            {
                BattleSub->HandleItemClicked(Item);
                return;
            }

            // 코인 클릭 시 그리드 역추적. 수정 필요(: 채널을 따로 만들든 등등...)
            if (ACoinActor *Coin = Cast<ACoinActor>(HitActor))
            {
                
                AGridActor *TargetGrid = GridSub->GetGridActor(Coin->GetDecidedGrid());
                if (TargetGrid)
                {
                    BattleSub->HandleGridClicked(TargetGrid);
                    return;
                }
            }
            // 그리드 직접 클릭
            if (AGridActor *TargetGrid = Cast<AGridActor>(HitActor))
            {
                BattleSub->HandleGridClicked(TargetGrid);
                return;
            }
        }
        // Camera에서 놓쳤을 경우 Visibility로 그리드 재체크
        if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
        {
            if (AGridActor *TargetGrid = Cast<AGridActor>(Hit.GetActor()))
            {
                BattleSub->HandleGridClicked(TargetGrid);
                return;
            }
        }
    }
    // CoinReadyTurn
    else if (CurrentTurn == ETurnState::CoinReadyTurn)
    {
        if (GetHitResultUnderCursor(ECC_Camera, false, Hit))
        {
            if (ACoinActor *ClickedCoin = Cast<ACoinActor>(Hit.GetActor()))
            {
                UCoinManagementWSubsystem *CoinSub = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
                if (CoinSub)
                {
                    int32 TargetID = ClickedCoin->GetCoinID();
                    if (CoinSub->IsCoinIdInBattleReady(TargetID))
                        CoinSub->RemoveBattleReadyCoins(ClickedCoin);
                    else if (ClickedCoin->GetSameTypeIndex() == 0)
                        CoinSub->AddBattleReadyCoins(ClickedCoin);
                    return;
                }
            }
        }
    }

    // 영역 이동 및 복귀
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
    ReturnToDefaultCamera();
}

// 우클릭: 디폴트 카메라 시점으로 복귀
void ABattlePlayerController_FlipSide::OnRightClick()
{
    UBattleManagerWSubsystem *BattleSub = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    if (BattleSub && BattleSub->GetCurrentTurn() == ETurnState::CoinSelectTurn)
    {
        BattleSub->HandleItemCanceled();
        BattleSub->HandleGridCanceled();
    }

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