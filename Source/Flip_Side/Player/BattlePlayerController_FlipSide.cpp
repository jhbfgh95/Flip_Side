// Fill out your copyright notice in the Description page of Project Settings.

#include "BattlePlayerController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "BattlePlayerPawn_FlipSide.h"
#include "BattleArea.h"
#include "BossActor.h"
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
#include "Subsystem/BattleLevel/UseableItemWSubsystem.h"
#include "Subsystem/BattleLevel/BattleLevelActingWSubsystem.h"
#include "Subsystem/BattleLevel/BossManagerSubsystem.h"
#include "Subsystem/StageCardWSubsystem.h"
#include "Subsystem/CursorGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/BattlePlayerHUDWidget.h"
#include "WeaponDataTypes.h"

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
		BattleManager->OnPhaseChanged.RemoveAll(this);
        BattleManager->OnPhaseChanged.AddDynamic(this, &ABattlePlayerController_FlipSide::OnPhaseChanged);
		BattleManager->OnBossPhaseCompleted.RemoveAll(this);
		BattleManager->OnBossPhaseCompleted.AddDynamic(this, &ABattlePlayerController_FlipSide::HandleBossPhaseCompleted);
		BattleManager->OnStageEnded.RemoveAll(this);
        BattleManager->OnStageEnded.AddDynamic(this, &ABattlePlayerController_FlipSide::OnStageEnded);
    }

    if (UBattleLevelActingWSubsystem* Acting = GetWorld()->GetSubsystem<UBattleLevelActingWSubsystem>())
    {
        Acting->OnBossDeadAct.BindUObject(this, &ABattlePlayerController_FlipSide::MoveCameraForBossDead);
    }

    if (BattleHUDWidgetClass)
    {
        BattleHUDWidget = CreateWidget<UBattlePlayerHUDWidget>(this, BattleHUDWidgetClass);
        if (IsValid(BattleHUDWidget))
        {
            BattleHUDWidget->AddToViewport();
            BattleHUDWidget->OnCoinSlotClicked.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBattleCoinSlotClicked);
			BattleHUDWidget->OnReadyCoinClicked.AddUObject(this, &ABattlePlayerController_FlipSide::HandleReadyCoinClicked);
			BattleHUDWidget->OnItemSlotClicked.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBattleItemSlotClicked);
			BattleHUDWidget->OnPhaseProgressClicked.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBattlePhaseProgressClicked);
        }
    }

	RefreshBattlePhaseHUD();

	TryBindBossHUD();

    if (UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>())
    {
        CoinManager->OnBattleCoinDataChanged.AddDynamic(this, &ABattlePlayerController_FlipSide::RefreshBattleCoinHUD);
        RefreshBattleCoinHUD();
    }

	if (UUseableItemWSubsystem* ItemManager = GetWorld()->GetSubsystem<UUseableItemWSubsystem>())
	{
		ItemManager->OnBattleItemDataChanged.AddDynamic(this, &ABattlePlayerController_FlipSide::RefreshBattleItemHUD);
		RefreshBattleItemHUD();
	}

	if (UStageCardWSubsystem* StageCardManager = GetWorld()->GetSubsystem<UStageCardWSubsystem>())
	{
		StageCardManager->OnBattleCardDataChanged.AddDynamic(this, &ABattlePlayerController_FlipSide::RefreshBattleCardHUD);
		RefreshBattleCardHUD();
	}
}

void ABattlePlayerController_FlipSide::TryBindBossHUD()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	UBossManagerSubsystem* BossManager = GetWorld()->GetSubsystem<UBossManagerSubsystem>();
	ABossActor* CurrentBoss = IsValid(BossManager) ? BossManager->GetCurrentBoss() : nullptr;
	if (!IsValid(CurrentBoss))
	{
		if (!GetWorldTimerManager().IsTimerActive(BossHUDBindRetryHandle))
		{
			GetWorldTimerManager().SetTimer(
				BossHUDBindRetryHandle,
				this,
				&ABattlePlayerController_FlipSide::TryBindBossHUD,
				0.1f,
				true
			);
		}
		return;
	}

	if (ObservedBoss != CurrentBoss)
	{
		if (IsValid(ObservedBoss))
		{
			ObservedBoss->OnBossHUDDataChanged.RemoveAll(this);
		}

		ObservedBoss = CurrentBoss;
		ObservedBoss->OnBossHUDDataChanged.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBossHUDDataChanged);
	}

	RefreshBossHUD();
	GetWorldTimerManager().ClearTimer(BossHUDBindRetryHandle);
}

void ABattlePlayerController_FlipSide::RefreshBossHUD()
{
	if (IsValid(BattleHUDWidget) && IsValid(ObservedBoss))
	{
		BattleHUDWidget->SetBossHUDData(ObservedBoss->GetBossHUDData());
	}
}

void ABattlePlayerController_FlipSide::HandleBossHUDDataChanged(const FBossHUDData& InData)
{
	if (IsValid(BattleHUDWidget))
	{
		BattleHUDWidget->SetBossHUDData(InData);
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

void ABattlePlayerController_FlipSide::OnPhaseChanged(EPhaseState NewPhase)
{
	RefreshBattlePhaseHUD();

    if (!ControlledPawn) return;

    if (NewPhase == EPhaseState::CoinBehaviorPhase)
    {
        GetWorldTimerManager().SetTimer(CoinBehaviorCameraDelayHandle, [this]()
        {
            if (ControlledPawn)
                ControlledPawn->MoveCameraToArea(CoinBehaviorCameraLocation, CoinBehaviorCameraRotation, CoinBehaviorCameraArmLength);
        }, CoinBehaviorCameraDelay, false);
    }
    else if (NewPhase == EPhaseState::CoinReadyPhase)
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

void ABattlePlayerController_FlipSide::HandleBossPhaseCompleted()
{
	if (IsValid(BattleHUDWidget))
	{
		BattleHUDWidget->PlayBossPhaseCompletionAnimation();
	}
}

void ABattlePlayerController_FlipSide::RefreshBattlePhaseHUD()
{
	if (!IsValid(BattleHUDWidget) || !IsValid(GetWorld()))
	{
		return;
	}

	UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
	if (!IsValid(BattleManager))
	{
		return;
	}

	BattleHUDWidget->SetPhaseDisplay(BattleManager->GetCurrentPhase(), BattleManager->GetTurnCount());
}

void ABattlePlayerController_FlipSide::RefreshBattleCoinHUD()
{
    if (!IsValid(BattleHUDWidget) || !IsValid(GetWorld()))
    {
        return;
    }

    UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
    if (!IsValid(CoinManager))
    {
        return;
    }

    TArray<FBattleCoinSlotViewData> CoinSlotViews;
    for (const FBattleCoinSlotData& CoinSlotData : CoinManager->GetCoinSlots())
    {
        CoinSlotViews.Add(BuildCoinSlotViewData(CoinSlotData));
    }
    BattleHUDWidget->SetCoinSlots(CoinSlotViews);

    TArray<FBattleReadyCoinViewData> ReadyCoinViews;
    const TArray<FReadyCoinData>& ReadyCoinData = CoinManager->GetReadyCoinData();
    for (int32 ReadyCoinIndex = 0; ReadyCoinIndex < ReadyCoinData.Num(); ++ReadyCoinIndex)
    {
        ReadyCoinViews.Add(BuildReadyCoinViewData(ReadyCoinData[ReadyCoinIndex], ReadyCoinIndex + 1));
    }
    BattleHUDWidget->SetReadyCoins(ReadyCoinViews);
}

void ABattlePlayerController_FlipSide::RefreshBattleItemHUD()
{
	if (!IsValid(BattleHUDWidget) || !IsValid(GetWorld()))
	{
		return;
	}

	UUseableItemWSubsystem* ItemManager = GetWorld()->GetSubsystem<UUseableItemWSubsystem>();
	if (!IsValid(ItemManager))
	{
		return;
	}

	const bool bCanUse = ItemManager->IsItemUseAvailable();
	TArray<FBattleItemSlotViewData> ItemSlotViews;
	for (const FBattleItemSlotData& ItemSlotData : ItemManager->GetBattleItemSlots())
	{
		ItemSlotViews.Add(BuildItemSlotViewData(ItemSlotData, bCanUse));
	}

	BattleHUDWidget->SetItemSlots(ItemSlotViews);
}

void ABattlePlayerController_FlipSide::RefreshBattleCardHUD()
{
	if (!IsValid(BattleHUDWidget) || !IsValid(GetWorld()))
	{
		return;
	}

	UStageCardWSubsystem* StageCardManager = GetWorld()->GetSubsystem<UStageCardWSubsystem>();
	if (!IsValid(StageCardManager))
	{
		return;
	}

	TArray<FBattleCardSlotViewData> CardSlotViews;
	StageCardManager->GetBattleCardSlots(CardSlotViews);
	BattleHUDWidget->SetCardSlots(CardSlotViews);
}

void ABattlePlayerController_FlipSide::HandleBattleCoinSlotClicked(int32 SlotNumber)
{
    if (!IsValid(GetWorld()))
    {
        return;
    }

    if (UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>())
    {
        CoinManager->TryAddReadyCoinFromSlot(SlotNumber);
    }
}

void ABattlePlayerController_FlipSide::HandleReadyCoinClicked(int32 CoinInstanceID)
{
    if (!IsValid(GetWorld()))
    {
        return;
    }

    if (UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>())
    {
        CoinManager->TryCancelReadyCoin(CoinInstanceID);
    }
}

void ABattlePlayerController_FlipSide::HandleBattleItemSlotClicked(int32 ItemID)
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	if (UUseableItemWSubsystem* ItemManager = GetWorld()->GetSubsystem<UUseableItemWSubsystem>())
	{
		ItemManager->TrySelectItem(ItemID);
	}
}

void ABattlePlayerController_FlipSide::HandleBattlePhaseProgressClicked()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	if (UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>())
	{
		// TODO: UI 애니메이션 완료 시점이 확정되면 그 콜백에서 요청하도록 변경합니다.
		BattleManager->RequestPhaseProgress();
	}
}

FBattleCoinSlotViewData ABattlePlayerController_FlipSide::BuildCoinSlotViewData(const FBattleCoinSlotData& CoinSlotData) const
{
    FBattleCoinSlotViewData ViewData;
    ViewData.SlotNumber = CoinSlotData.SlotNumber;
    ViewData.CoinCount = CoinSlotData.AvailableCoinCount;
    ViewData.HP = CoinSlotData.HP;
    ViewData.FrontWeaponID = CoinSlotData.FrontWeaponID;
    ViewData.BackWeaponID = CoinSlotData.BackWeaponID;

    UDataManagerSubsystem* DataManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
    if (!IsValid(DataManager))
    {
        return ViewData;
    }

    FFaceData FrontWeaponData;
    if (DataManager->TryGetWeapon(CoinSlotData.FrontWeaponID, FrontWeaponData))
    {
        ViewData.FrontIcon = FrontWeaponData.WeaponIcon;
    }

    FFaceData BackWeaponData;
    if (DataManager->TryGetWeapon(CoinSlotData.BackWeaponID, BackWeaponData))
    {
        ViewData.BackIcon = BackWeaponData.WeaponIcon;
    }

    return ViewData;
}

FBattleReadyCoinViewData ABattlePlayerController_FlipSide::BuildReadyCoinViewData(const FReadyCoinData& ReadyCoinData, int32 ReadySlotNumber) const
{
    FBattleReadyCoinViewData ViewData;
    ViewData.ReadySlotNumber = ReadySlotNumber;
    ViewData.CoinInstanceID = ReadyCoinData.CoinInstanceID;
    ViewData.CurrentHP = ReadyCoinData.CurrentHP;
    ViewData.bCanCancel = ReadyCoinData.bCanCancel;

    UDataManagerSubsystem* DataManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
    if (!IsValid(DataManager))
    {
        return ViewData;
    }

    FFaceData FrontWeaponData;
    if (DataManager->TryGetWeapon(ReadyCoinData.FrontWeaponID, FrontWeaponData))
    {
        ViewData.FrontIcon = FrontWeaponData.WeaponIcon;
    }

    FFaceData BackWeaponData;
    if (DataManager->TryGetWeapon(ReadyCoinData.BackWeaponID, BackWeaponData))
    {
        ViewData.BackIcon = BackWeaponData.WeaponIcon;
    }

    return ViewData;
}

FBattleItemSlotViewData ABattlePlayerController_FlipSide::BuildItemSlotViewData(const FBattleItemSlotData& ItemSlotData, bool bCanUse) const
{
	FBattleItemSlotViewData ViewData;
	ViewData.ItemData = ItemSlotData.ItemData;
	ViewData.AvailableCount = ItemSlotData.AvailableCount;
	ViewData.bCanUse = bCanUse;
	return ViewData;
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
