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
#include "Subsystem/BattleLevel/CoinActionManagementWSubsystem.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "Subsystem/BattleLevel/BattleLevelActingWSubsystem.h"
#include "Subsystem/CursorGISubsystem.h"

ABattlePlayerController_FlipSide::ABattlePlayerController_FlipSide()
{
    bShowMouseCursor = true;
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

    bIsUIOnly = false;
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    if (ControlledPawn)
    {
        // 디폴트 카메라 시점
        DefaultCameraLocation = FVector(-3086.0f, -990.0f, 2438.0f);
        DefaultCameraRotation = FRotator(-21.6f, 0.0f, 0.0f);
        DefaultCameraArmLength = 0.0f;
    }

    if (UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>())
    {
        BattleManager->OnTurnChanged.AddDynamic(this, &ABattlePlayerController_FlipSide::OnTurnChanged);
        BattleManager->OnStageEnded.AddDynamic(this, &ABattlePlayerController_FlipSide::OnStageEnded);
    }

    if (UBattleLevelActingWSubsystem* Acting = GetWorld()->GetSubsystem<UBattleLevelActingWSubsystem>())
    {
        Acting->OnBossDeadAct.BindUObject(this, &ABattlePlayerController_FlipSide::MoveCameraForBossDead);
    }
}

void ABattlePlayerController_FlipSide::ReturnToDefaultCamera() // 일단 당장은 필요 X
{
    if (ControlledPawn)
    {
        ControlledPawn->MoveCameraToArea(DefaultCameraLocation, DefaultCameraRotation, DefaultCameraArmLength);
    }
}

void ABattlePlayerController_FlipSide::OnLeftClick()
{
    if (bIsUIOnly)
    {
        return;
    }

    if (UCursorGISubsystem* CursorSys = GetGameInstance()->GetSubsystem<UCursorGISubsystem>())
    {
        CursorSys->SetCursorState(2);
        GetWorldTimerManager().SetTimer(CursorClickResetHandle, [this]()
        {
            if (UCursorGISubsystem* CS = GetGameInstance()->GetSubsystem<UCursorGISubsystem>())
                CS->SetCursorState(0);
        }, 0.15f, false);
    }

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
}

void ABattlePlayerController_FlipSide::CheckMouseHover()
{
    if (bIsUIOnly)
    {
        if (LastHoveredActor)
        {
            if (IBattleHoverInterface* PrevHover = Cast<IBattleHoverInterface>(LastHoveredActor))
            {
                PrevHover->Execute_OnUnhover(LastHoveredActor);
            }
            LastHoveredActor = nullptr;
        }

        if (CurrentHoveredArea)
        {
            CurrentHoveredArea->SetHighlight(false);
            CurrentHoveredArea = nullptr;
        }

        return;
    }

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

    // 커서 상태 업데이트 (클릭 애니메이션 중엔 덮어쓰지 않음)
    if (!GetWorldTimerManager().IsTimerActive(CursorClickResetHandle))
    {
        if (UCursorGISubsystem* CursorSys = GetGameInstance()->GetSubsystem<UCursorGISubsystem>())
        {
            const bool bHovering = CurrentActor && CurrentActor->Implements<UBattleHoverInterface>();
            CursorSys->SetCursorState(bHovering ? 1 : 0);
        }
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
    if (bIsUIOnly)
    {
        return;
    }

    if (UCursorGISubsystem* CursorSys = GetGameInstance()->GetSubsystem<UCursorGISubsystem>())
    {
        CursorSys->SetCursorState(2);
        GetWorldTimerManager().SetTimer(CursorClickResetHandle, [this]()
        {
            if (UCursorGISubsystem* CS = GetGameInstance()->GetSubsystem<UCursorGISubsystem>())
                CS->SetCursorState(0);
        }, 0.15f, false);
    }

    if (UCoinActionManagementWSubsystem* CoinActionManager = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>())
    {
        CoinActionManager->TryCancelCurrentAction();
    }
}

void ABattlePlayerController_FlipSide::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<ABattlePlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);
}

void ABattlePlayerController_FlipSide::MoveCameraForBossDead()
{
    if (ControlledPawn)
        ControlledPawn->MoveCameraToArea(BossDeadCameraLocation, BossDeadCameraRotation, BossDeadCameraArmLength);
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

void ABattlePlayerController_FlipSide::SetInputForTutorial(bool bEnable)
{
    bIsUIOnly = bEnable;

    if (bEnable)
    {
        SetInputMode(FInputModeUIOnly());
    }
    else
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);
    }
}

void ABattlePlayerController_FlipSide::OnStageEnded(int32 StageEndFlag)
{
    SetInputForTutorial(true);
}

bool ABattlePlayerController_FlipSide::GetCursorWorldLocationOnPlane(float PlaneZ, FVector& OutWorldLocation) const
{
    FVector WorldOrigin;
    FVector WorldDirection;

    if(!DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
    {
        return false;
    }

    if(FMath::IsNearlyZero(WorldDirection.Z))
    {
        return false;
    }

    const float Distance = (PlaneZ - WorldOrigin.Z) / WorldDirection.Z;
    if(Distance < 0.f)
    {
        return false;
    }

    OutWorldLocation = WorldOrigin + WorldDirection * Distance;
    return true;
}
