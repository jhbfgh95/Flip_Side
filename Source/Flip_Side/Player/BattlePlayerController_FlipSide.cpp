// Fill out your copyright notice in the Description page of Project Settings.

#include "BattlePlayerController_FlipSide.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
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
#include "ItemDataTypes.h"
#include "Actors/Component_Status.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    const FLinearColor BattleInfoFrontWeaponColor(0.862745f, 0.913725f, 0.313725f, 1.0f);
    const FLinearColor BattleInfoBackWeaponColor(0.905882f, 0.933333f, 0.917647f, 1.0f);

    FWeaponFaceStats BuildTemporaryFaceStatsForUI(const FFaceData& LegacyFaceData)
    {
        FWeaponFaceStats FaceStats;
        FaceStats.WeaponID = LegacyFaceData.WeaponID;
        FaceStats.BaseNumericStats.AttackPoint = FMath::Max(0, LegacyFaceData.AttackPoint);
        FaceStats.BaseNumericStats.WeaponPoint = FMath::Max(0, LegacyFaceData.BehaviorPoint);
        FaceStats.BaseNumericStats.WeaponCnt = 0;
        FaceStats.WeaponAttackSpec = LegacyFaceData.AttackAreaSpec;
        // TODO: DB에 능력 전용 범위가 추가되면 기존 단일 범위 공유를 교체합니다.
        FaceStats.WeaponBehaviorSpec = LegacyFaceData.AttackAreaSpec;
        return FaceStats;
    }

    int32 CalculateReadyCoinMaxHP(const FReadyCoinData& ReadyCoinData)
    {
        int64 MaxHP = FMath::Max(1, ReadyCoinData.BaseMaxHP);
        for (const FStatusEffectInstance& StatusEffect : ReadyCoinData.PersistentStatusEffects)
        {
            MaxHP += StatusEffect.Modifier.MaxHP;
        }
        return static_cast<int32>(FMath::Clamp<int64>(MaxHP, 1, MAX_int32));
    }
}

ABattlePlayerController_FlipSide::ABattlePlayerController_FlipSide()
{
    bShowMouseCursor = true;

    static ConstructorHelpers::FObjectFinder<UInputAction> ShowAdditionalBuffsActionFinder(
        TEXT("/Game/Player/Player_Battle/Inupts/IA_ShowAdditionalBuffs.IA_ShowAdditionalBuffs"));
    if (ShowAdditionalBuffsActionFinder.Succeeded())
    {
        ShowAdditionalBuffsInputAction = ShowAdditionalBuffsActionFinder.Object;
    }
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

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    if (IsValid(EnhancedInputComponent) && IsValid(ShowAdditionalBuffsInputAction))
    {
        EnhancedInputComponent->BindAction(
            ShowAdditionalBuffsInputAction,
            ETriggerEvent::Started,
            this,
            &ABattlePlayerController_FlipSide::HandleShowAdditionalBuffsStarted
        );
        EnhancedInputComponent->BindAction(
            ShowAdditionalBuffsInputAction,
            ETriggerEvent::Completed,
            this,
            &ABattlePlayerController_FlipSide::HandleShowAdditionalBuffsCompleted
        );
        EnhancedInputComponent->BindAction(
            ShowAdditionalBuffsInputAction,
            ETriggerEvent::Canceled,
            this,
            &ABattlePlayerController_FlipSide::HandleShowAdditionalBuffsCompleted
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[BattlePlayerController] IA_ShowAdditionalBuffs 또는 EnhancedInputComponent가 유효하지 않습니다."));
    }

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
            BattleHUDWidget->OnReadyCoinHovered.AddUObject(this, &ABattlePlayerController_FlipSide::HandleReadyCoinHovered);
            BattleHUDWidget->OnReadyCoinUnhovered.AddUObject(this, &ABattlePlayerController_FlipSide::HandleReadyCoinUnhovered);
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

void ABattlePlayerController_FlipSide::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopObservingBattleInfoCoin();
    HoveredBattleCoin.Reset();
    HoveredReadyCoinInstanceID = INDEX_NONE;
    if (IsValid(BattleHUDWidget))
    {
        BattleHUDWidget->HideBattleCoinInfo();
    }
    Super::EndPlay(EndPlayReason);
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

        EndBattleCoinActorHover();
        HoveredReadyCoinInstanceID = INDEX_NONE;
        if (IsValid(BattleHUDWidget))
        {
            BattleHUDWidget->HideBattleCoinInfo();
        }

        if (CurrentHoveredArea)
        {
            CurrentHoveredArea->SetHighlight(false);
            CurrentHoveredArea = nullptr;
        }

        return;
    }

    // ReadyCoinSlot 호버 중에는 UI 뒤의 월드 CoinActor를 중복 호버하지 않습니다.
    if (HoveredReadyCoinInstanceID != INDEX_NONE)
    {
        if (IsValid(LastHoveredActor))
        {
            if (IBattleHoverInterface* PreviousHover = Cast<IBattleHoverInterface>(LastHoveredActor))
            {
                PreviousHover->Execute_OnUnhover(LastHoveredActor);
            }
            LastHoveredActor = nullptr;
        }
        EndBattleCoinActorHover();
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
            if (ACoinActor* PreviousCoin = Cast<ACoinActor>(LastHoveredActor))
            {
                EndBattleCoinActorHover(PreviousCoin);
            }
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
            if (ACoinActor* CurrentCoin = Cast<ACoinActor>(CurrentActor);
                IsValid(CurrentCoin) && CurrentCoin->GetCoinOnBattle())
            {
                BeginBattleCoinActorHover(CurrentCoin);
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

    if (NewPhase == EPhaseState::CoinReadyPhase)
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

    if (HoveredReadyCoinInstanceID != INDEX_NONE)
    {
        RefreshHoveredBattleCoinInfo();
    }
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

void ABattlePlayerController_FlipSide::HandleReadyCoinHovered(int32 CoinInstanceID)
{
    if (CoinInstanceID == INDEX_NONE)
    {
        return;
    }

    if (IsValid(LastHoveredActor))
    {
        if (IBattleHoverInterface* PreviousHover = Cast<IBattleHoverInterface>(LastHoveredActor))
        {
            PreviousHover->Execute_OnUnhover(LastHoveredActor);
        }
        LastHoveredActor = nullptr;
    }
    EndBattleCoinActorHover();

    HoveredReadyCoinInstanceID = CoinInstanceID;
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::HandleReadyCoinUnhovered(int32 CoinInstanceID)
{
    if (CoinInstanceID == INDEX_NONE || HoveredReadyCoinInstanceID != CoinInstanceID)
    {
        return;
    }

    HoveredReadyCoinInstanceID = INDEX_NONE;
    StopObservingBattleInfoCoin();
    if (IsValid(BattleHUDWidget))
    {
        BattleHUDWidget->HideBattleCoinInfo();
    }
}

void ABattlePlayerController_FlipSide::HandleShowAdditionalBuffsStarted(
    const FInputActionValue& InputActionValue)
{
    bShowAdditionalBuffsHeld = InputActionValue.Get<bool>();
    if (IsValid(BattleHUDWidget) &&
        (HoveredReadyCoinInstanceID != INDEX_NONE || HoveredBattleCoin.IsValid()))
    {
        BattleHUDWidget->SetAdditionalBattleCoinBuffsVisible(bShowAdditionalBuffsHeld);
    }

    if (IsValid(BattleHUDWidget))
    {
        // 이 Shift IA는 레디/필드 코인의 추가 버프와 코인 슬롯 설명의 상세 스탯을 같이 제어합니다.
        BattleHUDWidget->SetCoinSlotDetailedStatsVisible(bShowAdditionalBuffsHeld);
    }
}

void ABattlePlayerController_FlipSide::HandleShowAdditionalBuffsCompleted(
    const FInputActionValue& InputActionValue)
{
    static_cast<void>(InputActionValue);
    bShowAdditionalBuffsHeld = false;
    if (IsValid(BattleHUDWidget))
    {
        BattleHUDWidget->SetAdditionalBattleCoinBuffsVisible(false);
        BattleHUDWidget->SetCoinSlotDetailedStatsVisible(false);
    }
}

void ABattlePlayerController_FlipSide::BeginBattleCoinActorHover(ACoinActor* CoinActor)
{
    if (!IsValid(CoinActor) || !CoinActor->GetCoinOnBattle())
    {
        return;
    }

    HoveredBattleCoin = CoinActor;
    ObserveBattleInfoCoin(CoinActor);
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::EndBattleCoinActorHover(ACoinActor* ExpectedCoin)
{
    ACoinActor* CurrentHoveredCoin = HoveredBattleCoin.Get();
    if (IsValid(ExpectedCoin) && CurrentHoveredCoin != ExpectedCoin)
    {
        return;
    }

    HoveredBattleCoin.Reset();
    if (HoveredReadyCoinInstanceID == INDEX_NONE)
    {
        StopObservingBattleInfoCoin();
        if (IsValid(BattleHUDWidget))
        {
            BattleHUDWidget->HideBattleCoinInfo();
        }
    }
}

void ABattlePlayerController_FlipSide::ObserveBattleInfoCoin(ACoinActor* CoinActor)
{
    if (ObservedBattleInfoCoin.Get() == CoinActor)
    {
        return;
    }

    StopObservingBattleInfoCoin();
    if (!IsValid(CoinActor) || !IsValid(CoinActor->StatComponent))
    {
        return;
    }

    ObservedBattleInfoCoin = CoinActor;
    ObservedBattleInfoStatus = CoinActor->StatComponent;

    UComponent_Status* StatusComponent = ObservedBattleInfoStatus.Get();
    StatusComponent->OnWeaponStatsChanged.AddUObject(
        this, &ABattlePlayerController_FlipSide::HandleObservedWeaponStatsChanged);
    StatusComponent->OnStatusEffectsChanged.AddUObject(
        this, &ABattlePlayerController_FlipSide::HandleObservedStatusEffectsChanged);
    StatusComponent->OnHpChanged.AddUObject(
        this, &ABattlePlayerController_FlipSide::HandleObservedHPChanged);
    StatusComponent->OnMaxHPChanged.AddUObject(
        this, &ABattlePlayerController_FlipSide::HandleObservedMaxHPChanged);
    StatusComponent->OnShieldChanged.AddUObject(
        this, &ABattlePlayerController_FlipSide::HandleObservedShieldChanged);
    CoinActor->OnCoinDeathStarted.AddUObject(
        this, &ABattlePlayerController_FlipSide::HandleObservedCoinDeath);
}

void ABattlePlayerController_FlipSide::StopObservingBattleInfoCoin()
{
    if (UComponent_Status* StatusComponent = ObservedBattleInfoStatus.Get())
    {
        StatusComponent->OnWeaponStatsChanged.RemoveAll(this);
        StatusComponent->OnStatusEffectsChanged.RemoveAll(this);
        StatusComponent->OnHpChanged.RemoveAll(this);
        StatusComponent->OnMaxHPChanged.RemoveAll(this);
        StatusComponent->OnShieldChanged.RemoveAll(this);
    }
    if (ACoinActor* CoinActor = ObservedBattleInfoCoin.Get())
    {
        CoinActor->OnCoinDeathStarted.RemoveAll(this);
    }
    ObservedBattleInfoStatus.Reset();
    ObservedBattleInfoCoin.Reset();
}

void ABattlePlayerController_FlipSide::RefreshHoveredBattleCoinInfo()
{
    if (!IsValid(BattleHUDWidget) || !IsValid(GetWorld()))
    {
        return;
    }

    FBattleCoinInfoViewData ViewData;
    if (HoveredReadyCoinInstanceID != INDEX_NONE)
    {
        UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
        if (!IsValid(CoinManager))
        {
            BattleHUDWidget->HideBattleCoinInfo();
            return;
        }

        const TArray<FReadyCoinData>& ReadyCoins = CoinManager->GetReadyCoinData();
        const int32 ReadyCoinIndex = ReadyCoins.IndexOfByPredicate([this](const FReadyCoinData& ReadyCoin)
        {
            return ReadyCoin.CoinInstanceID == HoveredReadyCoinInstanceID;
        });
        if (!ReadyCoins.IsValidIndex(ReadyCoinIndex))
        {
            HoveredReadyCoinInstanceID = INDEX_NONE;
            StopObservingBattleInfoCoin();
            BattleHUDWidget->HideBattleCoinInfo();
            return;
        }

        ACoinActor* RuntimeCoin = CoinManager->GetRuntimeCoinAtReadySlot(ReadyCoinIndex);
        const bool bBuiltFromRuntimeCoin = IsValid(RuntimeCoin) &&
            BuildBattleCoinInfoFromActor(RuntimeCoin, ViewData);
        if (bBuiltFromRuntimeCoin)
        {
            ObserveBattleInfoCoin(RuntimeCoin);
        }
        else
        {
            StopObservingBattleInfoCoin();
            if (!BuildBattleCoinInfoFromReadyData(ReadyCoins[ReadyCoinIndex], ViewData))
            {
                BattleHUDWidget->HideBattleCoinInfo();
                return;
            }
        }

        BattleHUDWidget->ShowBattleCoinInfo(ViewData, true);
        BattleHUDWidget->SetAdditionalBattleCoinBuffsVisible(bShowAdditionalBuffsHeld);
        return;
    }

    ACoinActor* FieldCoin = HoveredBattleCoin.Get();
    if (IsValid(FieldCoin) && BuildBattleCoinInfoFromActor(FieldCoin, ViewData))
    {
        ObserveBattleInfoCoin(FieldCoin);
        BattleHUDWidget->ShowBattleCoinInfo(ViewData, false);
        BattleHUDWidget->SetAdditionalBattleCoinBuffsVisible(bShowAdditionalBuffsHeld);
        return;
    }

    StopObservingBattleInfoCoin();
    BattleHUDWidget->HideBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::HandleObservedWeaponStatsChanged(
    const FWeaponStatsChangedEvent& ChangedEvent)
{
    static_cast<void>(ChangedEvent);
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::HandleObservedStatusEffectsChanged(
    const FStatusEffectsChangedEvent& ChangedEvent)
{
    static_cast<void>(ChangedEvent);
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::HandleObservedHPChanged(int32 DeltaHP)
{
    static_cast<void>(DeltaHP);
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::HandleObservedMaxHPChanged(int32 DeltaMaxHP)
{
    static_cast<void>(DeltaMaxHP);
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::HandleObservedShieldChanged(int32 DeltaShield)
{
    static_cast<void>(DeltaShield);
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::HandleObservedCoinDeath(ACoinActor* DeadCoin)
{
    if (HoveredBattleCoin.Get() == DeadCoin)
    {
        HoveredBattleCoin.Reset();
    }
    HoveredReadyCoinInstanceID = INDEX_NONE;
    StopObservingBattleInfoCoin();
    if (IsValid(BattleHUDWidget))
    {
        BattleHUDWidget->HideBattleCoinInfo();
    }
}

bool ABattlePlayerController_FlipSide::BuildBattleCoinInfoFromActor(
    ACoinActor* CoinActor,
    FBattleCoinInfoViewData& OutViewData) const
{
    if (!IsValid(CoinActor) || !IsValid(CoinActor->StatComponent))
    {
        return false;
    }

    UComponent_Status* StatusComponent = CoinActor->StatComponent;
    OutViewData = FBattleCoinInfoViewData();
    OutViewData.CoinInstanceID = CoinActor->GetCoinID();
    OutViewData.CurrentHP = StatusComponent->GetHP();
    OutViewData.MaxHP = StatusComponent->GetMaxHP();
    OutViewData.Shield = StatusComponent->GetShield();

    const bool bFrontValid = BuildWeaponFaceInfo(
        CoinActor->GetCoinFrontID(),
        StatusComponent->ResolveFaceStats(EFaceState::Front),
        BattleInfoFrontWeaponColor,
        OutViewData.FrontFace
    );
    const bool bBackValid = BuildWeaponFaceInfo(
        CoinActor->GetCoinBackID(),
        StatusComponent->ResolveFaceStats(EFaceState::Back),
        BattleInfoBackWeaponColor,
        OutViewData.BackFace
    );
    BuildStatusEffectViewData(StatusComponent->GetStatusEffects(), OutViewData.StatusEffects);
    return bFrontValid && bBackValid;
}

bool ABattlePlayerController_FlipSide::BuildBattleCoinInfoFromReadyData(
    const FReadyCoinData& ReadyCoinData,
    FBattleCoinInfoViewData& OutViewData) const
{
    UDataManagerSubsystem* DataManager = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>()
        : nullptr;
    if (!IsValid(DataManager) || ReadyCoinData.CoinInstanceID == INDEX_NONE)
    {
        return false;
    }

    FFaceData FrontWeaponData;
    FFaceData BackWeaponData;
    if (!DataManager->TryGetWeapon(ReadyCoinData.FrontWeaponID, FrontWeaponData) ||
        !DataManager->TryGetWeapon(ReadyCoinData.BackWeaponID, BackWeaponData))
    {
        return false;
    }

    const FResolvedWeaponFaceStats FrontStats = UComponent_Status::ResolveFaceStatsFromData(
        BuildTemporaryFaceStatsForUI(FrontWeaponData), ReadyCoinData.PersistentStatusEffects);
    const FResolvedWeaponFaceStats BackStats = UComponent_Status::ResolveFaceStatsFromData(
        BuildTemporaryFaceStatsForUI(BackWeaponData), ReadyCoinData.PersistentStatusEffects);

    OutViewData = FBattleCoinInfoViewData();
    OutViewData.CoinInstanceID = ReadyCoinData.CoinInstanceID;
    OutViewData.CurrentHP = ReadyCoinData.CurrentHP;
    OutViewData.MaxHP = CalculateReadyCoinMaxHP(ReadyCoinData);
    OutViewData.Shield = ReadyCoinData.Shield;

    const bool bFrontValid = BuildWeaponFaceInfo(
        ReadyCoinData.FrontWeaponID,
        FrontStats,
        BattleInfoFrontWeaponColor,
        OutViewData.FrontFace
    );
    const bool bBackValid = BuildWeaponFaceInfo(
        ReadyCoinData.BackWeaponID,
        BackStats,
        BattleInfoBackWeaponColor,
        OutViewData.BackFace
    );
    BuildStatusEffectViewData(ReadyCoinData.PersistentStatusEffects, OutViewData.StatusEffects);
    return bFrontValid && bBackValid;
}

bool ABattlePlayerController_FlipSide::BuildWeaponFaceInfo(
    int32 WeaponID,
    const FResolvedWeaponFaceStats& ResolvedStats,
    const FLinearColor& WeaponColor,
    FBattleWeaponFaceInfoViewData& OutFaceInfo) const
{
    UDataManagerSubsystem* DataManager = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>()
        : nullptr;
    if (!IsValid(DataManager))
    {
        return false;
    }

    FFaceData WeaponData;
    if (!DataManager->TryGetWeapon(WeaponID, WeaponData) || !IsValid(WeaponData.WeaponIcon))
    {
        return false;
    }

    OutFaceInfo.WeaponID = WeaponID;
    OutFaceInfo.WeaponIcon = WeaponData.WeaponIcon;
    OutFaceInfo.WeaponName = FText::FromString(WeaponData.WeaponName);
    OutFaceInfo.WeaponDescription = FText::FromString(WeaponData.KOR_DES);
    OutFaceInfo.BaseStats = ResolvedStats.BaseNumericStats;
    OutFaceInfo.FinalStats = ResolvedStats.FinalNumericStats;
    OutFaceInfo.WeaponColor = WeaponColor;
    return true;
}

void ABattlePlayerController_FlipSide::BuildStatusEffectViewData(
    const TArray<FStatusEffectInstance>& StatusEffects,
    TArray<FBattleStatusEffectViewData>& OutStatusEffects) const
{
    OutStatusEffects.Reset();
    UDataManagerSubsystem* DataManager = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>()
        : nullptr;
    if (!IsValid(DataManager))
    {
        return;
    }

    for (const FStatusEffectInstance& StatusEffect : StatusEffects)
    {
        FBattleStatusEffectViewData* ExistingView = OutStatusEffects.FindByPredicate(
            [&StatusEffect](const FBattleStatusEffectViewData& ViewData)
            {
                return ViewData.BuffTypeID == StatusEffect.BuffTypeID &&
                    ViewData.SourceType == StatusEffect.SourceType &&
                    ViewData.SourceDataID == StatusEffect.SourceDataID &&
                    ViewData.Polarity == StatusEffect.Polarity;
            });
        if (ExistingView)
        {
            ++ExistingView->StackCount;
            continue;
        }

        UTexture2D* SourceIcon = nullptr;
        switch (StatusEffect.SourceType)
        {
        case EStatusEffectSourceType::Coin:
        {
            FFaceData WeaponData;
            if (DataManager->TryGetWeapon(StatusEffect.SourceDataID, WeaponData))
            {
                SourceIcon = WeaponData.WeaponIcon;
            }
            break;
        }
        case EStatusEffectSourceType::Item:
        {
            FItemData ItemData;
            if (DataManager->TryGetItem(StatusEffect.SourceDataID, ItemData))
            {
                SourceIcon = ItemData.ItemIcon;
            }
            break;
        }
        case EStatusEffectSourceType::Boss:
            // TODO: 보스 주체 버프와 아이콘 데이터가 생기면 기존 DataManager 조회 API를 이 분기에 연결합니다.
            break;
        default:
            break;
        }

        if (!IsValid(SourceIcon))
        {
            continue;
        }

        FBattleStatusEffectViewData& NewViewData = OutStatusEffects.AddDefaulted_GetRef();
        NewViewData.BuffTypeID = StatusEffect.BuffTypeID;
        NewViewData.SourceType = StatusEffect.SourceType;
        NewViewData.SourceDataID = StatusEffect.SourceDataID;
        NewViewData.Polarity = StatusEffect.Polarity;
        NewViewData.Icon = SourceIcon;
        NewViewData.StackCount = 1;
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
    ViewData.FrontWeaponDisplay = CoinSlotData.FrontWeaponDisplay;
    ViewData.BackWeaponDisplay = CoinSlotData.BackWeaponDisplay;
    ViewData.FrontWeaponColor = BattleInfoFrontWeaponColor;
    ViewData.BackWeaponColor = BattleInfoBackWeaponColor;

    /*
     * TODO(DB_KEYWORD_RECONNECT): DB/DataManager 팀 작업 후 아래 연결을 복구합니다.
     * - 무기 ID로 토큰 설명 + AttackPower/WeaponPower/Count 조회
     * - 활성 키워드 일괄 조회 결과를 포매터 캐시에 전달
     * - AttackRange/AbilityRange Spec은 TODO(DB_RANGE_PREVIEW_RECONNECT) 완성 전까지 요청하지 않음
     *
     * FWeaponDescriptionDisplayData FrontDisplayData;
     * FWeaponDescriptionDisplayData BackDisplayData;
     * DataManager->TryGetWeaponDescriptionDisplayData(CoinSlotData.FrontWeaponID, FrontDisplayData);
     * DataManager->TryGetWeaponDescriptionDisplayData(CoinSlotData.BackWeaponID, BackDisplayData);
     */

    UDataManagerSubsystem* DataManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
    if (!IsValid(DataManager))
    {
        return ViewData;
    }

    FFaceData FrontWeaponData;
    if (DataManager->TryGetWeapon(CoinSlotData.FrontWeaponID, FrontWeaponData))
    {
        ViewData.FrontIcon = FrontWeaponData.WeaponIcon;
        ViewData.FrontWeaponName = FText::FromString(FrontWeaponData.WeaponName);
    }

    FFaceData BackWeaponData;
    if (DataManager->TryGetWeapon(CoinSlotData.BackWeaponID, BackWeaponData))
    {
        ViewData.BackIcon = BackWeaponData.WeaponIcon;
        ViewData.BackWeaponName = FText::FromString(BackWeaponData.WeaponName);
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
