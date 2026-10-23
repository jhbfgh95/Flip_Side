// Fill out your copyright notice in the Description page of Project Settings.

#include "BattlePlayerController_FlipSide.h"
#include "Actors/WeaponRangePreviewActor.h"
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
#include "Subsystem/BattleLevel/BattleTutorialWSubsystem.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Actors/Others/Turret_OtherActor.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "Subsystem/BattleLevel/UseableItemWSubsystem.h"
#include "Subsystem/BattleLevel/BattleLevelActingWSubsystem.h"
#include "Subsystem/BattleLevel/BossManagerSubsystem.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
#include "Subsystem/StageCardWSubsystem.h"
#include "Subsystem/CursorGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/BattlePlayerHUDWidget.h"
#include "UI/CoinInfoPinHintWidget.h"
#include "UI/CoinDescriptionFormatter.h"
#include "WeaponDataTypes.h"
#include "ItemDataTypes.h"
#include "Actors/Component_Status.h"
#include "Actors/DebuffComponent.h"
#include "Actors/CoinAttackRangeIndicatorActor.h"
#include "Actors/AbilityRangeActor.h"
#include "Actors/Boss/BossCoinActor.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "Framework/Application/SlateApplication.h"

namespace
{
    const FLinearColor BattleInfoFrontWeaponColor(0.862745f, 0.913725f, 0.313725f, 1.0f);
    const FLinearColor BattleInfoBackWeaponColor(0.905882f, 0.933333f, 0.917647f, 1.0f);

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
    InputComponent->BindKey(EKeys::MiddleMouseButton, IE_Pressed, this, &ABattlePlayerController_FlipSide::OnMiddleClick);

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
    // UI 위에서도 고정한 코인의 위치/면 변화와 행동 종료를 범위에 반영합니다.
    if (ACoinActor* PreviewCoin = TutorialRangePreviewCoin.IsValid() ? TutorialRangePreviewCoin.Get() : HoveredBattleCoin.Get(); !bIsUIOnly && IsValid(PreviewCoin))
    {
        UWorld* World = GetWorld();
        UCoinActionManagementWSubsystem* ActionManager = IsValid(World)
            ? World->GetSubsystem<UCoinActionManagementWSubsystem>() : nullptr;
        UBossManagerSubsystem* BossManager = IsValid(World)
            ? World->GetSubsystem<UBossManagerSubsystem>() : nullptr;
        ABossActor* Boss = IsValid(BossManager) ? BossManager->GetCurrentBoss() : nullptr;
        if (bBattleCoinInfoPinned && IsValid(ActionManager) && ActionManager->IsActionSequenceActive())
        {
            // 행동 중에는 실제 선택 대상의 범위를 우선하고, 종료 후 고정한 범위를 복원합니다.
            if (RangePreviewCoinCell.GridX >= 0) HideBattleCoinRangePreviews(PreviewCoin, true);
        }
        else if (!(RangePreviewCoinCell == PreviewCoin->GetDecidedGrid()) ||
            RangePreviewCoinFace != PreviewCoin->GetCoinDecidedFace() ||
            bRangePreviewHasBoss != IsValid(Boss) || RangePreviewBoss.Get() != Boss ||
            !PreviewCoin->GetCoinOnBattle())
        {
            if (bBattleCoinInfoPinned && RangePreviewCoinCell.GridX < 0)
            {
                // 행동 종료로 사라진 코인/보스 강조도 고정한 코인의 호버 상태로 복원합니다.
                IBattleHoverInterface::Execute_OnHover(PreviewCoin);
            }
            RefreshBattleCoinRangePreviews();
        }
    }
    UpdateCoinInfoPinHint();
    UpdateTurretRangePreview();
    UpdateActionAbilityRangePreview();
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
		PreviousPhase = BattleManager->GetCurrentPhase();
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

    if (IsLocalController())
    {
        if (IsValid(BattleHUDWidgetClass.Get()) && !BattleHUDWidgetClass->HasAnyClassFlags(CLASS_Abstract))
        {
            BattleHUDWidget = CreateWidget<UBattlePlayerHUDWidget>(this, BattleHUDWidgetClass);
            if (IsValid(BattleHUDWidget))
            {
                BattleHUDWidget->OnCoinSlotClicked.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBattleCoinSlotClicked);
                BattleHUDWidget->OnCoinSlotHovered.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBattleCoinSlotHovered);
                // 사거리 프리뷰는 슬롯 Unhover가 아니라 인포의 명시적 닫기까지 유지합니다.
                BattleHUDWidget->OnCoinSlotInfoDismissed.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBattleCoinSlotUnhovered);
                BattleHUDWidget->OnReadyCoinClicked.AddUObject(this, &ABattlePlayerController_FlipSide::HandleReadyCoinClicked);
                BattleHUDWidget->OnReadyCoinHovered.AddUObject(this, &ABattlePlayerController_FlipSide::HandleReadyCoinHovered);
                BattleHUDWidget->OnReadyCoinUnhovered.AddUObject(this, &ABattlePlayerController_FlipSide::HandleReadyCoinUnhovered);
                BattleHUDWidget->OnBattleInfoSelectionReset.AddUObject(this, &ABattlePlayerController_FlipSide::ResetBattleInfoSelection);
                BattleHUDWidget->OnReadySlotHighlightClearRequested.AddUObject(this, &ABattlePlayerController_FlipSide::ClearReadySlotHighlight);
                BattleHUDWidget->OnItemSlotClicked.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBattleItemSlotClicked);
                BattleHUDWidget->OnPhaseProgressClicked.AddUObject(this, &ABattlePlayerController_FlipSide::HandleBattlePhaseProgressClicked);
                BattleHUDWidget->AddToViewport();
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[BattleHUD] Failed to create the battle HUD."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[BattleHUD] No usable BattleHUDWidgetClass is assigned."));
        }
    }

	SpawnBattleRangePreviewActors();

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
    ClearReadySlotHighlight();
    ResetBattleInfoSelection();
    if (IsValid(CoinInfoPinHintWidget))
    {
        CoinInfoPinHintWidget->RemoveFromParent();
        CoinInfoPinHintWidget = nullptr;
    }
    if (IsValid(BattleHUDWidget))
    {
        BattleHUDWidget->OnCoinSlotHovered.RemoveAll(this);
        BattleHUDWidget->DismissCoinSlotInfo();
        BattleHUDWidget->OnCoinSlotInfoDismissed.RemoveAll(this);
        BattleHUDWidget->OnBattleInfoSelectionReset.RemoveAll(this);
        BattleHUDWidget->OnReadySlotHighlightClearRequested.RemoveAll(this);
        BattleHUDWidget->OnReadyCoinClicked.RemoveAll(this);
        BattleHUDWidget->OnReadyCoinHovered.RemoveAll(this);
        BattleHUDWidget->OnReadyCoinUnhovered.RemoveAll(this);
    }
    HandleBattleCoinSlotUnhovered(HoveredPreviewSlot);
    HideBattleCoinRangePreviews();
    StopObservingBattleInfoCoin();
    HoveredBattleCoin.Reset();
    HoveredReadyCoinInstanceID = INDEX_NONE;
    if (IsValid(BattleHUDWidget))
    {
        BattleHUDWidget->HideBattleCoinInfo();
    }

	if (IsValid(AttackRangeIndicatorActor))
	{
		AttackRangeIndicatorActor->Destroy();
		AttackRangeIndicatorActor = nullptr;
	}
	if (IsValid(AbilityRangeActor))
	{
		AbilityRangeActor->Destroy();
		AbilityRangeActor = nullptr;
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
		HandleBossHUDDataChanged(ObservedBoss->GetBossHUDData());
	}
}

void ABattlePlayerController_FlipSide::HandleBossHUDDataChanged(const FBossHUDData& InData)
{
	if (IsValid(BattleHUDWidget))
	{
		FBossHUDData DisplayData = InData;
		FBossPatternBattleData PatternData;
		// 표시용 조회만 수행합니다. BossPatternBase의 보호막 회복 분기와 같은 배율입니다.
		if (InData.bHasPatternInfo && IsValid(ObservedBoss) &&
			ObservedBoss->GetPatternData(InData.PatternDisplayIndex - 1, PatternData) &&
			PatternData.bNoDamage && PatternData.GimmickType == EBossGimmickType::Shield && PatternData.ShieldHeal > 0)
		{
			DisplayData.bShowPatternShieldHeal = true;
			DisplayData.PatternShieldHeal = FMath::RoundToInt(PatternData.ShieldHeal * ObservedBoss->GetStageMultiplierStat());
		}
		BattleHUDWidget->SetBossHUDData(DisplayData);
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
	if (const ULevelGISubsystem* Level = GetGameInstance()->GetSubsystem<ULevelGISubsystem>(); IsValid(Level) && Level->IsBattleTutorialActive())
	{
		const auto* Tutorial = GetWorld()->GetSubsystem<UBattleTutorialWSubsystem>();
		if (!IsValid(Tutorial) || !Tutorial->IsTutorialWorldClickAllowed()) return;
	}
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
    if (bBattleCoinInfoPinned)
    {
        ACoinActor* PinnedCoin = HoveredBattleCoin.Get();
        if (!IsValid(PinnedCoin) || !PinnedCoin->GetCoinOnBattle() ||
            !IsValid(PinnedCoin->StatComponent) || PinnedCoin->StatComponent->GetHP() <= 0)
        {
            ClearBattleCoinInfoPin();
        }
    }
    // 커서가 실제로 움직였을 때만 3D 오브젝트 호버 소리를 냅니다(코인 착지·카메라 이동으로 대상이 바뀐 경우 제외).
    USoundManagerWSubsystem* SoundManager = GetWorld()->GetSubsystem<USoundManagerWSubsystem>();
    const bool bHoverCursorMoved = SoundManager && SoundManager->PollCursorMoved();

    if (bIsUIOnly)
    {
        ClearBattleCoinInfoPin();
        if (IsValid(LastHoveredActor))
        {
            NotifyBattleActorHover(LastHoveredActor, false);
            LastHoveredActor = nullptr;
        }

        EndBattleCoinActorHover();
        ClearReadySlotHighlight();

        if (CurrentHoveredArea)
        {
            CurrentHoveredArea->SetHighlight(false);
            CurrentHoveredArea = nullptr;
        }

        return;
    }

    // ReadyCoinSlot 호버 중에는 UI 뒤의 월드 CoinActor를 중복 호버하지 않습니다.
    const bool bPointerOverHUD = IsValid(BattleHUDWidget) && FSlateApplication::IsInitialized() &&
        BattleHUDWidget->GetCoinPopupPointerRegion(FSlateApplication::Get().GetCursorPos()) != ECoinPopupPointerRegion::World;
    if (HoveredReadyCoinInstanceID != INDEX_NONE || bPointerOverHUD)
    {
        if (IsValid(LastHoveredActor))
        {
            NotifyBattleActorHover(LastHoveredActor, false);
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

    // 행동 중인 코인은 커서 아래에 있어도 팝업/호버 연출 대상으로 삼지 않습니다.
    if (ACoinActor* Coin = Cast<ACoinActor>(CurrentActor); IsValid(Coin) && Coin->GetCoinIsActing())
    {
        CurrentActor = nullptr;
    }

	if (const ULevelGISubsystem* Level = GetGameInstance()->GetSubsystem<ULevelGISubsystem>(); IsValid(Level) && Level->IsBattleTutorialActive())
		if (ACoinActor* Coin = Cast<ACoinActor>(CurrentActor))
		{
			const auto* Tutorial = GetWorld()->GetSubsystem<UBattleTutorialWSubsystem>();
			if (!IsValid(Tutorial) || !Tutorial->CanHoverTutorialCoin(Coin)) CurrentActor = nullptr;
		}
    if (LastHoveredActor != CurrentActor)
    {
        if (IsValid(LastHoveredActor))
        {
            if (ACoinActor* PreviousCoin = Cast<ACoinActor>(LastHoveredActor))
            {
                EndBattleCoinActorHover(PreviousCoin);
            }
            NotifyBattleActorHover(LastHoveredActor, false);
        }

        if (IsValid(CurrentActor))
        {
            NotifyBattleActorHover(CurrentActor, true);
            if (ACoinActor* CurrentCoin = Cast<ACoinActor>(CurrentActor);
                IsValid(CurrentCoin) && CurrentCoin->GetCoinOnBattle())
            {
                BeginBattleCoinActorHover(CurrentCoin);
            }

            // 전투 코인과 포탑은 버튼처럼 마우스를 올리면 소리를 냅니다. 바닥 칸은 너무 자주 지나가서 뺍니다.
            bool bPlayHoverSound = false;
            if (ACoinActor* HoverCoin = Cast<ACoinActor>(CurrentActor))
            {
                bPlayHoverSound = !bBattleCoinInfoPinned && HoverCoin->GetCoinOnBattle();
            }
            else if (ATurret_OtherActor* HoverTurret = Cast<ATurret_OtherActor>(CurrentActor))
            {
                UCoinActionManagementWSubsystem* ActionManager = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>();
                bPlayHoverSound = HoverTurret->GetHP() > 0 && !(IsValid(ActionManager) && ActionManager->IsActionSequenceActive());
            }
            if (bPlayHoverSound && bHoverCursorMoved)
            {
                SoundManager->PlayHoverSound();
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

bool ABattlePlayerController_FlipSide::TryCancelSelectedItem()
{
    if (bIsUIOnly || !IsValid(GetWorld()) || !IsValid(GetGameInstance())) return false;
    const ULevelGISubsystem* Level = GetGameInstance()->GetSubsystem<ULevelGISubsystem>();
    if (IsValid(Level) && Level->IsBattleTutorialActive()) return false;

    UUseableItemWSubsystem* ItemManager = GetWorld()->GetSubsystem<UUseableItemWSubsystem>();
    if (!IsValid(ItemManager) || ItemManager->GetSelectedItemID() == INDEX_NONE) return false;

    ItemManager->CancelWantUseItem();
    return true;
}

// 우클릭: 선택 중인 아이템을 먼저 취소하고, 선택이 없으면 코인 행동을 취소합니다.
void ABattlePlayerController_FlipSide::OnRightClick()
{
	if (const ULevelGISubsystem* Level = GetGameInstance()->GetSubsystem<ULevelGISubsystem>(); IsValid(Level) && Level->IsBattleTutorialActive()) return;
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

    if (TryCancelSelectedItem()) return;

    if (UCoinActionManagementWSubsystem* CoinActionManager = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>())
    {
        CoinActionManager->TryCancelCurrentAction();
    }
}

ACoinActor* ABattlePlayerController_FlipSide::GetPinnableBattleCoinUnderCursor(int32& OutReadySlot) const
{
    OutReadySlot = INDEX_NONE;
    if (bIsUIOnly || !IsValid(GetWorld()) || !IsValid(GetGameInstance()) || !IsValid(BattleHUDWidget) ||
        !BattleHUDWidget->IsVisible() || !FSlateApplication::IsInitialized()) return nullptr;
    const ULevelGISubsystem* Level = GetGameInstance()->GetSubsystem<ULevelGISubsystem>();
    if (IsValid(Level) && Level->IsBattleTutorialActive()) return nullptr;
    if (BattleHUDWidget->GetCoinPopupPointerRegion(FSlateApplication::Get().GetCursorPos()) != ECoinPopupPointerRegion::World)
        return nullptr;

    const UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    const UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
    if (!IsValid(BattleManager) || BattleManager->GetCurrentPhase() != EPhaseState::CoinBehaviorPhase ||
        !IsValid(CoinManager)) return nullptr;

    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Camera, true, Hit)) return nullptr;
    ACoinActor* Coin = Cast<ACoinActor>(Hit.GetActor());
    if (!IsValid(Coin) || !Coin->GetCoinOnBattle() || Coin->GetCoinIsActing() || Coin->IsHidden() ||
        !IsValid(Coin->StatComponent) || Coin->StatComponent->GetHP() <= 0) return nullptr;
    const int32 SlotIndex = CoinManager->GetReadyCoinData().IndexOfByPredicate([Coin](const FReadyCoinData& Data)
    { return Data.CoinInstanceID == Coin->GetCoinID(); });
    if (SlotIndex == INDEX_NONE || CoinManager->GetRuntimeCoinAtReadySlot(SlotIndex) != Coin) return nullptr;
    OutReadySlot = SlotIndex;
    return Coin;
}

void ABattlePlayerController_FlipSide::OnMiddleClick()
{
    if (bIsUIOnly || !IsValid(GetGameInstance()) || !IsValid(BattleHUDWidget) || !FSlateApplication::IsInitialized()) return;
    const ULevelGISubsystem* Level = GetGameInstance()->GetSubsystem<ULevelGISubsystem>();
    if (IsValid(Level) && Level->IsBattleTutorialActive()) return;
    // 정보창 등 UI에서 누른 휠 클릭은 월드의 고정 해제로 처리하지 않습니다.
    if (BattleHUDWidget->GetCoinPopupPointerRegion(FSlateApplication::Get().GetCursorPos()) != ECoinPopupPointerRegion::World) return;

    int32 ReadySlot = INDEX_NONE;
    ACoinActor* Coin = GetPinnableBattleCoinUnderCursor(ReadySlot);
    if (!IsValid(Coin) || (bBattleCoinInfoPinned && SelectedInfoCoinInstanceID == Coin->GetCoinID()))
    {
        ClearBattleCoinInfoPin();
        CheckMouseHover();
        return;
    }

    ClearBattleCoinInfoPin();
    CheckMouseHover();
    SelectedInfoCoinInstanceID = Coin->GetCoinID();
    SelectedInfoReadySlot = ReadySlot;
    SelectedInfoUpperFace = Coin->GetCoinDecidedFace();
    bInfoUsesStoredFront = false;
    bBattleCoinInfoPinned = true;
    ObserveBattleInfoCoin(Coin);
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::UpdateCoinInfoPinHint()
{
    int32 ReadySlot = INDEX_NONE;
    if (!IsValid(GetPinnableBattleCoinUnderCursor(ReadySlot)))
    {
        if (IsValid(CoinInfoPinHintWidget)) CoinInfoPinHintWidget->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }
    if (!IsValid(CoinInfoPinHintWidget))
    {
        if (!IsValid(CoinInfoPinHintWidgetClass.Get()) || CoinInfoPinHintWidgetClass->HasAnyClassFlags(CLASS_Abstract)) return;
        CoinInfoPinHintWidget = CreateWidget<UCoinInfoPinHintWidget>(this, CoinInfoPinHintWidgetClass);
        if (IsValid(CoinInfoPinHintWidget)) CoinInfoPinHintWidget->AddToViewport(1000);
    }
    if (IsValid(CoinInfoPinHintWidget)) CoinInfoPinHintWidget->UpdatePositionAtCursor();
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
    if (NewPhase != PreviousPhase) ClearBattleCoinInfoPin();
    ClearReadySlotHighlight();
    // 행동 페이즈에서 보스 페이즈로 넘어갈 때 정보 페이지와 선택을 함께 해제합니다.
    if (NewPhase == EPhaseState::BossPhase &&
        (PreviousPhase == EPhaseState::CoinBehaviorPhase || PreviousPhase == EPhaseState::BehaviorPhase))
    {
        ResetBattleInfoSelection();
    }
    PreviousPhase = NewPhase;
    RefreshHoveredBattleCoinInfo();

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
        ClearBattleCoinInfoPin();
        if (IsValid(CoinInfoPinHintWidget)) CoinInfoPinHintWidget->SetVisibility(ESlateVisibility::Collapsed);
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
        const int32 HoveredIndex = ReadyCoinData.IndexOfByPredicate([this](const FReadyCoinData& Data)
        { return Data.CoinInstanceID == HoveredReadyCoinInstanceID; });
        if (!ReadyCoinData.IsValidIndex(HoveredIndex) ||
            CoinManager->GetRuntimeCoinAtReadySlot(HoveredIndex) != HighlightedReadySlotCoin.Get())
            ClearReadySlotHighlight();
    }
    RefreshHoveredBattleCoinInfo();
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

	BattleHUDWidget->SetItemSlots(ItemSlotViews, ItemManager->GetSelectedItemID());
}

void ABattlePlayerController_FlipSide::TargetCardChange(int32 CardID, int32 CardSlot)
{
	UWorld* World = GetWorld();
	UStageCardWSubsystem* Cards = IsValid(World) ? World->GetSubsystem<UStageCardWSubsystem>() : nullptr;
	if (!IsValid(Cards))
	{
		UE_LOG(LogTemp, Warning, TEXT("[TargetCardChange] Battle card subsystem is unavailable."));
		return;
	}
	Cards->ChangeHandCard(CardID, CardSlot);
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

void ABattlePlayerController_FlipSide::HandleBattleCoinSlotHovered(int32 SlotNumber)
{
    HoveredPreviewSlot = SlotNumber;
    UWorld* World = GetWorld();
    if (!IsValid(World)) return;
    if (!SlotRangePreviewActor.IsValid())
    {
        // 레벨에 배치한 단일 PreviewActor를 사용하여 에디터의 월드 위치를 보존합니다.
        for (TActorIterator<AWeaponRangePreviewActor> It(World); It; ++It)
        {
            if (!IsValid(*It)) continue;
            if (SlotRangePreviewActor.IsValid())
            {
                UE_LOG(LogTemp, Warning, TEXT("Slot preview: multiple WeaponRangePreviewActors; keep only one in this level."));
                SlotRangePreviewActor.Reset();
                return;
            }
            SlotRangePreviewActor = *It;
        }
    }
    AWeaponRangePreviewActor* Preview = SlotRangePreviewActor.Get();
    if (!IsValid(Preview))
    {
        UE_LOG(LogTemp, Warning, TEXT("Slot preview: place BP_WeaponRangePreviewActor in the level."));
        return;
    }
    UCoinManagementWSubsystem* CoinManager = World->GetSubsystem<UCoinManagementWSubsystem>();
    UDataManagerSubsystem* DataManager = IsValid(GetGameInstance()) ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
    if (IsValid(CoinManager) && IsValid(DataManager))
    {
        for (const FBattleCoinSlotData& Slot : CoinManager->GetCoinSlots())
        {
            if (Slot.SlotNumber != SlotNumber) continue;
            FFaceData Front;
            FFaceData Back;
            if (DataManager->TryGetWeapon(Slot.FrontWeaponID, Front) && DataManager->TryGetWeapon(Slot.BackWeaponID, Back))
            {
                // 슬롯의 버프/캐시 스탯 대신 DB 정의에서만 기본 사거리를 읽습니다.
                Preview->ShowDefinitionPreview(Front.AttackAreaSpec, Front.AbilityAreaSpec, Front.bHasAbilityArea,
                    Back.AttackAreaSpec, Back.AbilityAreaSpec, Back.bHasAbilityArea);
                return;
            }
            break;
        }
    }
    Preview->ClearPreview();
    UE_LOG(LogTemp, Warning, TEXT("Slot preview: missing slot/weapon definition for slot %d."), SlotNumber);
}

void ABattlePlayerController_FlipSide::HandleBattleCoinSlotUnhovered(int32 SlotNumber)
{
    // HUD의 인포 닫기 이벤트에서만 호출합니다. 슬롯의 물리적인 Unhover와는 연결하지 않습니다.
    if (HoveredPreviewSlot != SlotNumber) return;
    HoveredPreviewSlot = INDEX_NONE;
    if (AWeaponRangePreviewActor* Preview = SlotRangePreviewActor.Get(); IsValid(Preview))
        Preview->ClearPreview();
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
    if (!IsValid(GetWorld()) || CoinInstanceID == INDEX_NONE) return;
    UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
    UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    if (!IsValid(CoinManager) || !IsValid(BattleManager)) return;
    const EPhaseState Phase = BattleManager->GetCurrentPhase();
    if (Phase != EPhaseState::CoinReadyPhase && Phase != EPhaseState::CoinBehaviorPhase && Phase != EPhaseState::BossPhase) return;
    const TArray<FReadyCoinData>& Coins = CoinManager->GetReadyCoinData();
    const int32 Index = Coins.IndexOfByPredicate([CoinInstanceID](const FReadyCoinData& Data)
    { return Data.CoinInstanceID == CoinInstanceID; });
    if (!Coins.IsValidIndex(Index)) return;
    if (Phase == EPhaseState::CoinReadyPhase && Coins[Index].bCanCancel)
    {
        CoinManager->TryCancelReadyCoin(CoinInstanceID);
        return;
    }
    ClearReadySlotHighlight();
    ResetBattleInfoSelection();
    SelectedInfoCoinInstanceID = CoinInstanceID;
    SelectedInfoReadySlot = Index;
    // Ready에서 여는 정보는 전투에서 결정된 윗면과 무관하게 저장된 Front부터 시작합니다.
    bInfoUsesStoredFront = Phase == EPhaseState::CoinReadyPhase;
    RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::HandleReadyCoinHovered(int32 CoinInstanceID)
{
    if (CoinInstanceID == INDEX_NONE)
    {
        return;
    }

    if (IsValid(LastHoveredActor))
    {
        NotifyBattleActorHover(LastHoveredActor, false);
        LastHoveredActor = nullptr;
    }
    EndBattleCoinActorHover();

    ClearReadySlotHighlight();
    HoveredReadyCoinInstanceID = CoinInstanceID;
    UCoinManagementWSubsystem* CoinManager = IsValid(GetWorld())
        ? GetWorld()->GetSubsystem<UCoinManagementWSubsystem>() : nullptr;
    if (!IsValid(CoinManager)) return;
    const int32 SlotIndex = CoinManager->GetReadyCoinData().IndexOfByPredicate([CoinInstanceID](const FReadyCoinData& Data)
    { return Data.CoinInstanceID == CoinInstanceID; });
    if (!CoinManager->GetReadyCoinData().IsValidIndex(SlotIndex)) return;
    UBattleManagerWSubsystem* BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    if (!IsValid(BattleManager)) return;
    const EPhaseState Phase = BattleManager->GetCurrentPhase();
    if (Phase == EPhaseState::CoinReadyPhase || Phase == EPhaseState::BossPhase)
    {
        const FReadyCoinData& Data = CoinManager->GetReadyCoinData()[SlotIndex];
        if (Data.bCanCancel && IsValid(BattleHUDWidget)) BattleHUDWidget->ShowReadyCoinSlotInfo(Data.SourceSlotNumber);
        return;
    }
    if (Phase != EPhaseState::CoinBehaviorPhase) return;
    ACoinActor* Coin = CoinManager->GetRuntimeCoinAtReadySlot(SlotIndex);
    if (!IsValid(Coin) || !IsValid(Coin->StatComponent) || Coin->StatComponent->GetHP() <= 0 || Coin->IsHidden()) return;
    HighlightedReadySlotCoin = Coin;
    // CoinActor가 슬롯 호버 색과 기존 Buff/Debuff 두께를 MPC/Stencil로 전달합니다.
    Coin->SetReadySlotHighlighted(true);
}

void ABattlePlayerController_FlipSide::HandleReadyCoinUnhovered(int32 CoinInstanceID)
{
    if (CoinInstanceID == INDEX_NONE || HoveredReadyCoinInstanceID != CoinInstanceID)
    {
        return;
    }

    // 우측 슬롯과 동일하게 팝업은 공통 입력 처리기가 닫습니다. 팝업으로 이동해도 유지합니다.
    if (ACoinActor* Coin = HighlightedReadySlotCoin.Get(); IsValid(Coin)) Coin->SetReadySlotHighlighted(false);
    HighlightedReadySlotCoin.Reset();
    HoveredReadyCoinInstanceID = INDEX_NONE;
}

void ABattlePlayerController_FlipSide::ClearReadySlotHighlight()
{
    if (IsValid(BattleHUDWidget)) BattleHUDWidget->HideReadyCoinSlotInfo();
    if (ACoinActor* Coin = HighlightedReadySlotCoin.Get(); IsValid(Coin)) Coin->SetReadySlotHighlighted(false);
    HighlightedReadySlotCoin.Reset();
    HoveredReadyCoinInstanceID = INDEX_NONE;
}

void ABattlePlayerController_FlipSide::ResetBattleInfoSelection()
{
    ClearBattleCoinInfoPin();
    // 사망 시 이 ID를 해제하므로 이후 같은 1~10 ID가 재사용되어도 다른 코인을 표시하지 않습니다.
    SelectedInfoCoinInstanceID = INDEX_NONE;
    SelectedInfoReadySlot = INDEX_NONE;
    SelectedInfoUpperFace = EFaceState::Front;
    bInfoUsesStoredFront = false;
    StopObservingBattleInfoCoin();
    if (IsValid(BattleHUDWidget)) BattleHUDWidget->HideBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::ClearBattleCoinInfoPin()
{
    if (!bBattleCoinInfoPinned) return;
    bBattleCoinInfoPinned = false;
    ACoinActor* PinnedCoin = HoveredBattleCoin.Get();
    if (IsValid(PinnedCoin)) NotifyBattleActorHover(PinnedCoin, false);
    EndBattleCoinActorHover(PinnedCoin);
    // 고정 중 생략했던 코인 호버를 다음 판정에서 실제 커서 위치로 다시 시작합니다.
    if (IsValid(LastHoveredActor) && LastHoveredActor->IsA<ACoinActor>()) LastHoveredActor = nullptr;
}

void ABattlePlayerController_FlipSide::NotifyBattleActorHover(AActor* Actor, bool bHovered)
{
    if (!IsValid(Actor)) return;
    if (ACoinActor* Coin = Cast<ACoinActor>(Actor); IsValid(Coin) && bBattleCoinInfoPinned)
    {
        // 호버 연출은 고정하되, 능력의 대상 선택은 실제 포인터 아래 코인을 사용합니다.
        UCoinActionManagementWSubsystem* ActionManager = IsValid(GetWorld())
            ? GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>() : nullptr;
        if (IsValid(ActionManager) && ActionManager->IsActionSequenceActive())
        {
            if (bHovered) ActionManager->SetSelectedWeapon(Coin);
            else ActionManager->HandleCoinUnHovered();
        }
        return;
    }
    if (Actor->Implements<UBattleHoverInterface>())
    {
        if (bHovered) IBattleHoverInterface::Execute_OnHover(Actor);
        else IBattleHoverInterface::Execute_OnUnhover(Actor);
    }
}

void ABattlePlayerController_FlipSide::HandleShowAdditionalBuffsStarted(
    const FInputActionValue& InputActionValue)
{
	static_cast<void>(InputActionValue);
	if (IsValid(BattleHUDWidget)) BattleHUDWidget->ToggleCoinDescriptionDetails();

}

void ABattlePlayerController_FlipSide::HandleShowAdditionalBuffsCompleted(
    const FInputActionValue& InputActionValue)
{
    static_cast<void>(InputActionValue);
    // 기존 IA 연결은 유지하며 상태 목록은 Shift와 무관하게 모두 표시합니다.
}

void ABattlePlayerController_FlipSide::BeginBattleCoinActorHover(ACoinActor* CoinActor)
{
    if (bBattleCoinInfoPinned || !IsValid(CoinActor) || !CoinActor->GetCoinOnBattle())
    {
        return;
    }

    HoveredBattleCoin = CoinActor;
    UBattleManagerWSubsystem* BattleManager = IsValid(GetWorld()) ? GetWorld()->GetSubsystem<UBattleManagerWSubsystem>() : nullptr;
    UCoinManagementWSubsystem* CoinManager = IsValid(GetWorld()) ? GetWorld()->GetSubsystem<UCoinManagementWSubsystem>() : nullptr;
    if (IsValid(BattleManager) && BattleManager->GetCurrentPhase() == EPhaseState::CoinBehaviorPhase &&
        IsValid(CoinManager) && IsValid(CoinActor->StatComponent) && CoinActor->StatComponent->GetHP() > 0)
    {
        const int32 SlotIndex = CoinManager->GetReadyCoinData().IndexOfByPredicate([CoinActor](const FReadyCoinData& Data)
        { return Data.CoinInstanceID == CoinActor->GetCoinID(); });
        if (SlotIndex != INDEX_NONE && CoinManager->GetRuntimeCoinAtReadySlot(SlotIndex) == CoinActor)
        {
            SelectedInfoCoinInstanceID = CoinActor->GetCoinID();
            bInfoUsesStoredFront = false;
            SelectedInfoReadySlot = SlotIndex;
            SelectedInfoUpperFace = CoinActor->GetCoinDecidedFace();
            ObserveBattleInfoCoin(CoinActor);
            RefreshHoveredBattleCoinInfo();
        }
    }
	ShowBattleCoinRangePreviews(CoinActor);
}

void ABattlePlayerController_FlipSide::EndBattleCoinActorHover(ACoinActor* ExpectedCoin)
{
    if (bBattleCoinInfoPinned) return;
    ACoinActor* CurrentHoveredCoin = HoveredBattleCoin.Get();
    if (IsValid(ExpectedCoin) && CurrentHoveredCoin != ExpectedCoin)
    {
        return;
    }

	HideBattleCoinRangePreviews(CurrentHoveredCoin);
    HoveredBattleCoin.Reset();
    // 범위 프리뷰만 끝냅니다. 정보 대상의 변경 이벤트 구독은 계속 유지합니다.
}

void ABattlePlayerController_FlipSide::SpawnBattleRangePreviewActors()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetPawn();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AttackRangeIndicatorClass)
	{
		AttackRangeIndicatorActor = World->SpawnActor<ACoinAttackRangeIndicatorActor>(
			AttackRangeIndicatorClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParameters
		);
		if (IsValid(AttackRangeIndicatorActor))
		{
			AttackRangeIndicatorActor->HideRange();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[BattlePlayerController] AttackRangeIndicatorClass가 BP에 지정되지 않았습니다."));
	}

	if (AbilityRangeActorClass)
	{
		AbilityRangeActor = World->SpawnActor<AAbilityRangeActor>(
			AbilityRangeActorClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParameters
		);
		if (IsValid(AbilityRangeActor))
		{
			AbilityRangeActor->HideRange();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[BattlePlayerController] AbilityRangeActorClass가 BP에 지정되지 않았습니다."));
	}
}

void ABattlePlayerController_FlipSide::RefreshBattleCoinRangePreviews()
{
	if (ACoinActor* CoinActor = TutorialRangePreviewCoin.Get(); IsValid(CoinActor))
	{
		ShowBattleCoinRangePreviews(CoinActor);
		return;
	}
	if (ACoinActor* CoinActor = HoveredBattleCoin.Get(); IsValid(CoinActor))
	{
		ShowBattleCoinRangePreviews(CoinActor);
	}
}

void ABattlePlayerController_FlipSide::SetTutorialRangePreviewCoin(ACoinActor* CoinActor, bool bAbilityRangeOnly)
{
	if (TutorialRangePreviewCoin.Get() == CoinActor && bTutorialAbilityRangeOnly == bAbilityRangeOnly) return;
	ACoinActor* Previous = TutorialRangePreviewCoin.Get();
	TutorialRangePreviewCoin.Reset();
	HideBattleCoinRangePreviews(Previous, true);
	TutorialRangePreviewCoin = CoinActor;
	bTutorialAbilityRangeOnly = IsValid(CoinActor) && bAbilityRangeOnly;
	if (IsValid(CoinActor)) ShowBattleCoinRangePreviews(CoinActor);
}

void ABattlePlayerController_FlipSide::ShowBattleCoinRangePreviews(ACoinActor* CoinActor)
{
	if (ACoinActor* TutorialCoin = TutorialRangePreviewCoin.Get(); IsValid(TutorialCoin)) CoinActor = TutorialCoin;
	UWorld* PreviewWorld = GetWorld();
	UCoinActionManagementWSubsystem* ActionManager = IsValid(PreviewWorld)
		? PreviewWorld->GetSubsystem<UCoinActionManagementWSubsystem>() : nullptr;
	if (IsValid(ActionManager) && ActionManager->IsActionSequenceActive()) return;
	HideBattleCoinRangePreviews(CoinActor, true);

	UWorld* World = GetWorld();
	if (!IsValid(CoinActor) || !CoinActor->GetCoinOnBattle() ||
		!IsValid(CoinActor->StatComponent) || !IsValid(World))
	{
		return;
	}

	UGridManagerSubsystem* GridManager = World->GetSubsystem<UGridManagerSubsystem>();
	if (!IsValid(GridManager))
	{
		return;
	}

	const FGridPoint CoinCell = CoinActor->GetDecidedGrid();
	const EFaceState CurrentFace = CoinActor->GetCoinDecidedFace();
	RangePreviewCoinCell = CoinCell;
	RangePreviewCoinFace = CurrentFace;
	UBossManagerSubsystem* BossManager = World->GetSubsystem<UBossManagerSubsystem>();
	ABossActor* CurrentBoss = IsValid(BossManager) ? BossManager->GetCurrentBoss() : nullptr;
	RangePreviewBoss = CurrentBoss;
	bRangePreviewHasBoss = IsValid(CurrentBoss);
	if (CoinCell.GridX < 0 || CoinCell.GridY < 0 || CurrentFace == EFaceState::None)
	{
		return;
	}

	// 실제 클릭 시 UWeapon_Action이 받는 것과 같은 최신 스탯 스냅숏으로 두 미리보기를 계산합니다.
	const FWeaponActionSnapshot PreviewSnapshot = CoinActor->StatComponent->BuildActionSnapshot(CurrentFace);
	if (PreviewSnapshot.WeaponID == INDEX_NONE)
	{
		return;
	}

	if (!bTutorialAbilityRangeOnly)
		CoinActor->SetAttackRangeBracketVisible(ShowAttackRangePreview(CoinCell, PreviewSnapshot.AttackAreaSpec));

	if (PreviewSnapshot.bHasAbilityArea && IsValid(AbilityRangeActor))
	{
		TArray<FGridPoint> AbilityCells;
		GridManager->BuildAbilityAreaCellsFromOrigin(
			CoinCell,
			PreviewSnapshot.AbilityAreaSpec,
			AbilityCells
		);

		TArray<FVector> AbilityWorldLocations;
		AbilityWorldLocations.Reserve(AbilityCells.Num());
		for (const FGridPoint& AbilityCell : AbilityCells)
		{
			FVector WorldLocation;
			if (GridManager->TryGetGridWorldLocation(AbilityCell, WorldLocation))
			{
				AbilityWorldLocations.Add(WorldLocation);
			}
		}
		AbilityRangeActor->ShowRangeAtWorldLocations(AbilityWorldLocations);
		// 클릭 직후 동일 범위라면 인스턴스를 지웠다 만들지 않고 그대로 인계합니다.
		AbilityRangePreviewLocations = MoveTemp(AbilityWorldLocations);
	}
}


bool ABattlePlayerController_FlipSide::ShowAttackRangePreview(const FGridPoint& Origin, const FAttackAreaSpec& Spec)
{
	UWorld* World = GetWorld();
	UGridManagerSubsystem* GridManager = IsValid(World) ? World->GetSubsystem<UGridManagerSubsystem>() : nullptr;
	if (!IsValid(GridManager)) return false;
	UBossManagerSubsystem* BossManager = World->GetSubsystem<UBossManagerSubsystem>();
	ABossActor* Boss = IsValid(BossManager) ? BossManager->GetCurrentBoss() : nullptr;
	RangePreviewBoss = Boss;
	bRangePreviewHasBoss = IsValid(Boss);
	bool bAttackRangeVisible = false;
	TArray<FGridPoint> AttackCells;
	ABossActor* AttackBoss = nullptr;
	GridManager->CollectAttackRangeTargets(Origin, Spec, AttackCells, AttackBoss);
	const bool bBossInRange = IsValid(AttackBoss);
	SetBossTargetArrowVisible(bBossInRange);
	if (IsValid(AttackRangeIndicatorActor))
	{
		FGridPoint AttackStartCell;
		FGridPoint AttackEndCell;
		if (GridManager->TryBuildStraightRangeEndpoints(
			Origin,
			Spec,
			bRangePreviewHasBoss,
			AttackStartCell,
			AttackEndCell))
		{
			FVector CoinWorldLocation;
			FVector AttackStartWorldLocation;
			FVector AttackEndWorldLocation;
			if (GridManager->TryGetGridWorldLocation(Origin, CoinWorldLocation) &&
				GridManager->TryGetGridWorldLocation(AttackStartCell, AttackStartWorldLocation) &&
				GridManager->TryGetGridWorldLocation(AttackEndCell, AttackEndWorldLocation))
			{
				// 셀 중심 사이의 간격은 방향으로만 쓰고, 실제 메시 경계는 Indicator가 계산합니다.
				const FVector GridStepWorld = AttackStartWorldLocation - CoinWorldLocation;

				bAttackRangeVisible = AttackRangeIndicatorActor->ShowRange(
					AttackStartWorldLocation,
					AttackEndWorldLocation,
					GridStepWorld,
					bBossInRange
				);
			}
		}
	}
	if (!bAttackRangeVisible && IsValid(AttackRangeIndicatorActor)) AttackRangeIndicatorActor->HideRange();
	return bAttackRangeVisible;
}

void ABattlePlayerController_FlipSide::UpdateTurretRangePreview()
{
	UWorld* World = GetWorld();
	UCoinActionManagementWSubsystem* ActionManager = IsValid(World)
		? World->GetSubsystem<UCoinActionManagementWSubsystem>() : nullptr;
	ATurret_OtherActor* Turret = !bIsUIOnly && !bBattleCoinInfoPinned && IsValid(LastHoveredActor)
		? Cast<ATurret_OtherActor>(LastHoveredActor) : nullptr;
	if (IsValid(ActionManager) && ActionManager->IsActionSequenceActive()) Turret = nullptr;
	if (IsValid(Turret) && Turret->GetHP() <= 0) Turret = nullptr;

	if (HoveredRangeTurret.Get() != Turret || (!IsValid(Turret) && bShowingTurretRange))
	{
		if (ATurret_OtherActor* Previous = HoveredRangeTurret.Get(); IsValid(Previous))
		{
			Previous->SetAttackRangeBracketVisible(false);
		}
		if (bShowingTurretRange)
		{
			HideBattleCoinRangePreviews();
		}
		HoveredRangeTurret = Turret;
		bShowingTurretRange = false;
		if (!IsValid(Turret)) RefreshBattleCoinRangePreviews();
	}
	if (IsValid(Turret))
	{
		if (!bShowingTurretRange) HideBattleCoinRangePreviews();
		Turret->SetAttackRangeBracketVisible(ShowAttackRangePreview(Turret->GetAttackOrigin(), Turret->GetAttackAreaSpec()));
		bShowingTurretRange = true;
	}
}

void ABattlePlayerController_FlipSide::HideBattleCoinRangePreviews(ACoinActor* CoinActor, bool bForce)
{
	// 튜토리얼 설명 중에는 마우스가 설명창으로 이동해도 강조할 범위를 유지합니다.
	if (!bForce && TutorialRangePreviewCoin.IsValid())
	{
		UWorld* World = GetWorld();
		const auto* Action = IsValid(World) ? World->GetSubsystem<UCoinActionManagementWSubsystem>() : nullptr;
		if (!IsValid(Action) || !Action->IsActionSequenceActive()) return;
	}
	RangePreviewCoinCell = FGridPoint{ -1, -1 };
	RangePreviewCoinFace = EFaceState::None;
	if (ATurret_OtherActor* Turret = HoveredRangeTurret.Get(); IsValid(Turret))
	{
		Turret->SetAttackRangeBracketVisible(false);
	}
	SetBossTargetArrowVisible(false);
	ACoinActor* TargetCoin = IsValid(CoinActor) ? CoinActor : HoveredBattleCoin.Get();
	if (IsValid(TargetCoin))
	{
		TargetCoin->SetAttackRangeBracketVisible(false);
	}

	if (IsValid(AttackRangeIndicatorActor))
	{
		AttackRangeIndicatorActor->HideRange();
	}

	TArray<FGridPoint> SelectionCells;
	UWorld* PreviewWorld = GetWorld();
	UCoinActionManagementWSubsystem* ActionManager = IsValid(PreviewWorld)
		? PreviewWorld->GetSubsystem<UCoinActionManagementWSubsystem>() : nullptr;
	const bool bKeepSelectionRange = !bIsUIOnly && IsValid(ActionManager) &&
		ActionManager->GetActiveAbilityPreviewCells(SelectionCells);
	if (IsValid(AbilityRangeActor) && !bKeepSelectionRange)
	{
		AbilityRangeActor->HideRange();
		bShowingActionAbilityRange = false;
		AbilityRangePreviewLocations.Reset();
	}
}

void ABattlePlayerController_FlipSide::UpdateActionAbilityRangePreview()
{
	UWorld* World = GetWorld();
	if (!IsValid(World) || !IsValid(AbilityRangeActor)) return;
	UCoinActionManagementWSubsystem* ActionManager = World->GetSubsystem<UCoinActionManagementWSubsystem>();
	UGridManagerSubsystem* GridManager = World->GetSubsystem<UGridManagerSubsystem>();
	TArray<FGridPoint> Cells;
	if (!bIsUIOnly && IsValid(ActionManager) && IsValid(GridManager) &&
		ActionManager->GetActiveAbilityPreviewCells(Cells, Cast<AGridActor>(LastHoveredActor)))
	{
		TArray<FVector> Locations;
		for (const FGridPoint& Cell : Cells)
		{
			FVector Location;
			if (GridManager->TryGetGridWorldLocation(Cell, Location)) Locations.Add(Location);
		}
		// 선택 대상 호버가 바뀌어도 시전자의 범위를 유지하고 변경 시에만 메시를 갱신합니다.
		if (AbilityRangePreviewLocations != Locations)
		{
			AbilityRangeActor->ShowRangeAtWorldLocations(Locations);
			AbilityRangePreviewLocations = MoveTemp(Locations);
		}
		bShowingActionAbilityRange = true;
	}
	else if (bShowingActionAbilityRange)
	{
		AbilityRangeActor->HideRange();
		bShowingActionAbilityRange = false;
		AbilityRangePreviewLocations.Reset();
		if (bBattleCoinInfoPinned)
			RefreshBattleCoinRangePreviews();
		else if (bTutorialAbilityRangeOnly && TutorialRangePreviewCoin.IsValid())
			ShowBattleCoinRangePreviews(TutorialRangePreviewCoin.Get());
	}
}

void ABattlePlayerController_FlipSide::SetBossTargetArrowVisible(bool bVisible)
{
	UWorld* World = GetWorld();
	if (bVisible && !RangePreviewBossCoin.IsValid() && IsValid(World))
	{
		for (TActorIterator<ABossCoinActor> It(World); It; ++It)
		{
			if (IsValid(*It))
			{
				RangePreviewBossCoin = *It;
				break;
			}
		}
	}
	if (ABossCoinActor* BossCoin = RangePreviewBossCoin.Get(); IsValid(BossCoin))
	{
		BossCoin->SetTargetArrowVisible(bVisible);
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
    if (!IsValid(BattleHUDWidget) || !IsValid(GetWorld()) || SelectedInfoCoinInstanceID == INDEX_NONE)
    {
        return;
    }

    UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
    if (!IsValid(CoinManager))
    {
        ResetBattleInfoSelection();
        return;
    }
    const TArray<FReadyCoinData>& ReadyCoins = CoinManager->GetReadyCoinData();
    if (!ReadyCoins.IsValidIndex(SelectedInfoReadySlot) ||
        ReadyCoins[SelectedInfoReadySlot].CoinInstanceID != SelectedInfoCoinInstanceID)
    {
        ResetBattleInfoSelection();
        return;
    }
    FBattleCoinInfoViewData ViewData;
    ACoinActor* RuntimeCoin = CoinManager->GetRuntimeCoinAtReadySlot(SelectedInfoReadySlot);
    if (IsValid(RuntimeCoin))
    {
        if (RuntimeCoin->GetCoinID() != SelectedInfoCoinInstanceID || !IsValid(RuntimeCoin->StatComponent) ||
            RuntimeCoin->StatComponent->GetHP() <= 0)
        {
            ResetBattleInfoSelection();
            return;
        }
        ObserveBattleInfoCoin(RuntimeCoin);
        if (!BuildBattleCoinInfoFromActor(RuntimeCoin, ViewData))
        {
            BattleHUDWidget->HideBattleCoinInfo();
            return;
        }
        if (!bInfoUsesStoredFront && RuntimeCoin->GetCoinDecidedFace() != EFaceState::None)
            SelectedInfoUpperFace = RuntimeCoin->GetCoinDecidedFace();
    }
    else
    {
        // 생존 액터를 정리한 SettingPhase 이후에는 매니저가 저장한 상태로 계속 표시합니다.
        StopObservingBattleInfoCoin();
        if (ReadyCoins[SelectedInfoReadySlot].CurrentHP <= 0 ||
            !BuildBattleCoinInfoFromReadyData(ReadyCoins[SelectedInfoReadySlot], ViewData))
        {
            ResetBattleInfoSelection();
            return;
        }
    }
    ViewData.UpperFace = SelectedInfoUpperFace;
    BattleHUDWidget->ShowBattleCoinInfo(ViewData);
}

void ABattlePlayerController_FlipSide::HandleObservedWeaponStatsChanged(
    const FWeaponStatsChangedEvent& ChangedEvent)
{
    static_cast<void>(ChangedEvent);
    RefreshHoveredBattleCoinInfo();
	RefreshBattleCoinRangePreviews();
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
    if (IsValid(DeadCoin) && DeadCoin->GetCoinID() == SelectedInfoCoinInstanceID) ResetBattleInfoSelection();
	HideBattleCoinRangePreviews(DeadCoin);
    if (HoveredBattleCoin.Get() == DeadCoin)
    {
        HoveredBattleCoin.Reset();
    }
    if (HighlightedReadySlotCoin.Get() == DeadCoin) ClearReadySlotHighlight();
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
    OutViewData.UpperFace = CoinActor->GetCoinDecidedFace();
    OutViewData.CurrentHP = StatusComponent->GetHP();
    OutViewData.MaxHP = StatusComponent->GetMaxHP();
    OutViewData.Shield = StatusComponent->GetShield();
    OutViewData.ShieldGaugeCapacity = StatusComponent->GetShieldGaugeCapacity();

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
        BuildWeaponFaceStatsFromDefinition(FrontWeaponData), ReadyCoinData.PersistentStatusEffects);
    const FResolvedWeaponFaceStats BackStats = UComponent_Status::ResolveFaceStatsFromData(
        BuildWeaponFaceStatsFromDefinition(BackWeaponData), ReadyCoinData.PersistentStatusEffects);

    OutViewData = FBattleCoinInfoViewData();
    OutViewData.CoinInstanceID = ReadyCoinData.CoinInstanceID;
    OutViewData.CurrentHP = ReadyCoinData.CurrentHP;
    OutViewData.MaxHP = CalculateReadyCoinMaxHP(ReadyCoinData);
    OutViewData.Shield = ReadyCoinData.Shield;
    OutViewData.ShieldGaugeCapacity = ReadyCoinData.ShieldGaugeCapacity;

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
    if (!DataManager->TryGetWeapon(WeaponID, WeaponData))
    {
        return false;
    }

    OutFaceInfo.WeaponID = WeaponID;
    OutFaceInfo.WeaponIcon = WeaponData.WeaponIcon;
    OutFaceInfo.WeaponName = FText::FromString(WeaponData.WeaponName);
    OutFaceInfo.WeaponDescription = FText::FromString(WeaponData.KOR_DES);
    TArray<FKeywordDefinitionData> Keywords;
    DataManager->GetAllEnabledKeywordDefinitions(Keywords);
    OutFaceInfo.Description = FCoinDescriptionFormatter::Parse(WeaponData.KOR_DES, Keywords, ResolvedStats.FinalNumericStats);
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
    const UBattleManagerWSubsystem* BattleManagerForTurn = GetWorld() ? GetWorld()->GetSubsystem<UBattleManagerWSubsystem>() : nullptr;
    const int32 CurrentTurn = IsValid(BattleManagerForTurn) ? BattleManagerForTurn->GetTurnCount() : 0;
    for (const FStatusEffectInstance& StatusEffect : StatusEffects)
    {
        // 타격은 키워드 내부 값입니다. 카드도 실제 스탯 수정치가 있는 효과만 목록에 표시합니다.
        if (StatusEffect.Polarity == EStatusPolarity::Buff && StatusEffect.BuffTypeID == WeaponBuffTypeID::Strike) continue;
        if (StatusEffect.SourceType == EStatusEffectSourceType::Card && StatusEffect.Modifier.IsZero()) continue;
        // 낙인(독)은 남은 턴이 아니라 "몇 턴째인지(찍힌 뒤 지난 턴 수)"를 표시합니다.
        const bool bIsBrand = StatusEffect.Polarity == EStatusPolarity::Debuff && StatusEffect.BuffTypeID == DebuffTypeID::Poison;
        // 낙인은 "그림 x N"으로 표시합니다. N은 다음 보스 페이즈에 곱해질 지난 턴 수이며 찍힌 직후에도 x1부터 시작합니다.
        const int32 DisplayTurns = bIsBrand ? FMath::Max(1, BrandDebuff::ElapsedTurns(StatusEffect, CurrentTurn)) : StatusEffect.RemainingTurns;
        // 낙인은 재타격마다 그림이 따로 추가되므로 같은 값이어도 하나로 합치지 않습니다.
        FBattleStatusEffectViewData* ExistingView = bIsBrand ? nullptr : OutStatusEffects.FindByPredicate(
            [&StatusEffect, DisplayTurns](const FBattleStatusEffectViewData& ViewData)
            {
                return ViewData.BuffTypeID == StatusEffect.BuffTypeID &&
                    ViewData.SourceType == StatusEffect.SourceType &&
                    ViewData.SourceDataID == StatusEffect.SourceDataID &&
                    ViewData.SourcePatternIndex == StatusEffect.SourcePatternIndex &&
                    ViewData.Polarity == StatusEffect.Polarity &&
                    ViewData.DurationType == StatusEffect.DurationType &&
                    ViewData.RemainingTurns == DisplayTurns &&
                    ViewData.CCType == StatusEffect.CCType;
            });
        if (ExistingView)
        {
            ++ExistingView->StackCount;
            continue;
        }

        UTexture2D* SourceIcon = nullptr;
        FText SourceName;
        FString Description;
        FDebuffDefinitionData DebuffDefinition;
        const bool bHasDebuffDefinition = StatusEffect.Polarity == EStatusPolarity::Debuff &&
            IsValid(DataManager) && DataManager->TryGetDebuff(StatusEffect.BuffTypeID, DebuffDefinition);
        switch (StatusEffect.SourceType)
        {
        case EStatusEffectSourceType::Coin:
        {
            FFaceData WeaponData;
            if (IsValid(DataManager) && DataManager->TryGetWeapon(StatusEffect.SourceDataID, WeaponData))
            {
                SourceIcon = WeaponData.WeaponIcon;
                SourceName = FText::FromString(WeaponData.WeaponName);
                Description = WeaponData.BuffInfo;
            }
            break;
        }
        case EStatusEffectSourceType::Item:
        {
            FItemData ItemData;
            if (IsValid(DataManager) && DataManager->TryGetItem(StatusEffect.SourceDataID, ItemData))
            {
                SourceIcon = ItemData.ItemIcon;
                SourceName = FText::FromString(ItemData.ItemName);
                Description = ItemData.BuffInfo;
            }
            break;
        }
        case EStatusEffectSourceType::Card:
        {
            FCardData CardData;
            if (IsValid(DataManager) && DataManager->TryGetCard(StatusEffect.SourceDataID, CardData))
            {
                SourceIcon = CardData.Icon;
                SourceName = FText::FromString(CardData.CardName);
            }
            // 카드 조건/추가 행동/흡혈은 섞지 않고 이 코인에 적용된 스탯만 표시합니다.
            const FWeaponStatModifier& Modifier = StatusEffect.Modifier;
            const TCHAR* Keys[] = { TEXT("AttackPower"), TEXT("WeaponPower"), TEXT("Count"),
                TEXT("AttackRange"), TEXT("AbilityRange") };
            const int32 Values[] = { Modifier.AttackPoint, Modifier.WeaponPoint, Modifier.WeaponCnt,
                Modifier.AttackRange, Modifier.AbilityRange };
            TArray<FString> Lines;
            for (int32 Index = 0; Index < UE_ARRAY_COUNT(Keys); ++Index)
                if (Values[Index] != 0) Lines.Add(FString::Printf(TEXT("[STAT:%s] %+d"), Keys[Index], Values[Index]));
            Description = FString::Join(Lines, TEXT("\n"));
            break;
        }
        case EStatusEffectSourceType::Boss:
            // 현재 보스 패턴을 재조회하지 않습니다. 부여 당시 아이콘을 유지합니다.
            SourceIcon = StatusEffect.SourcePatternIcon.Get();
            break;
        default:
            break;
        }

        FBattleStatusEffectViewData& NewViewData = OutStatusEffects.AddDefaulted_GetRef();
        NewViewData.BuffTypeID = StatusEffect.BuffTypeID;
        NewViewData.SourceType = StatusEffect.SourceType;
        NewViewData.SourceDataID = StatusEffect.SourceDataID;
        NewViewData.SourcePatternIndex = StatusEffect.SourcePatternIndex;
        NewViewData.Polarity = StatusEffect.Polarity;
        NewViewData.Icon = StatusEffect.Polarity == EStatusPolarity::Debuff ? DebuffDefinition.Icon.Get() : SourceIcon;
        NewViewData.DisplayName = SourceName;
        if (bHasDebuffDefinition)
        {
            NewViewData.DisplayName = DebuffDefinition.DisplayName;
            Description = DebuffDefinition.Description.ToString();
        }
        // 중첩 수를 곱하지 않습니다. 이 아이콘에 묶인 첫 번째 적용의 실제 값을 사용합니다.
        const FWeaponStatModifier& Modifier = StatusEffect.Modifier;
        const TCHAR* ValueKeys[] = { TEXT("{AttackPower}"), TEXT("{WeaponPower}"), TEXT("{Count}"),
            TEXT("{AttackRange}"), TEXT("{AbilityRange}"), TEXT("{Shield}"),
            TEXT("{Magnitude}"), TEXT("{DamageReduction}"), TEXT("{AttackPowerReduction}"), TEXT("{WeaponPowerReduction}") };
        const int32 Values[] = { Modifier.AttackPoint, Modifier.WeaponPoint, Modifier.WeaponCnt,
            Modifier.AttackRange + Modifier.AttackAreaSpec.ParamB, Modifier.AbilityRange + Modifier.AbilityAreaSpec.ParamB,
            StatusEffect.RuntimeValue, StatusEffect.ReactiveMagnitude, StatusEffect.ReactiveMagnitude,
            FMath::Abs(Modifier.AttackPoint), FMath::Abs(Modifier.WeaponPoint) };
        for (int32 Index = 0; Index < UE_ARRAY_COUNT(ValueKeys); ++Index)
            Description.ReplaceInline(ValueKeys[Index], *FText::AsNumber(Values[Index]).ToString(), ESearchCase::CaseSensitive);
        // 피격 전에는 증가량이 아직 정해지지 않았으므로 실제 계산식을 표시합니다.
        const FString NextTurnAttack = StatusEffect.BuffTypeID == WeaponBuffTypeID::ArmorNextTurnAttack
            ? FText::AsNumber(Modifier.AttackPoint).ToString() : TEXT("받은 공격 피해의 50%");
        Description.ReplaceInline(TEXT("{NextTurnAttack}"), *NextTurnAttack, ESearchCase::CaseSensitive);
        NewViewData.Description = FText::FromString(Description);
        if (bIsBrand)
        {
            NewViewData.bIsElapsedCounter = true;
            NewViewData.StackCount = DisplayTurns; // 그림 옆 "x N" 칸에 지난 턴 수를 표시
        }
        if (!bIsBrand) NewViewData.StackCount = 1;
        NewViewData.RemainingTurns = bIsBrand ? 0 : DisplayTurns;
        NewViewData.DurationType = StatusEffect.DurationType;
        NewViewData.CCType = StatusEffect.CCType;
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
    ViewData.FrontWeaponStats = CoinSlotData.FrontWeaponStats;
    ViewData.BackWeaponStats = CoinSlotData.BackWeaponStats;
    ViewData.FrontWeaponColor = BattleInfoFrontWeaponColor;
    ViewData.BackWeaponColor = BattleInfoBackWeaponColor;

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
		TArray<FKeywordDefinitionData> Keywords;
		DataManager->GetAllEnabledKeywordDefinitions(Keywords);
		// 슬롯 설명은 DB 기본값만 사용하며 CoinActor/StatusComponent를 조회하지 않습니다.
		const FWeaponNumericStats BaseStats{FrontWeaponData.AttackPoint, FrontWeaponData.BehaviorPoint, FrontWeaponData.Count};
		ViewData.FrontWeaponStats = {BaseStats.AttackPoint, BaseStats.WeaponPoint, BaseStats.WeaponCnt};
		ViewData.FrontDescription = FCoinDescriptionFormatter::Parse(FrontWeaponData.KOR_DES, Keywords, BaseStats);
    }

    FFaceData BackWeaponData;
    if (DataManager->TryGetWeapon(CoinSlotData.BackWeaponID, BackWeaponData))
    {
        ViewData.BackIcon = BackWeaponData.WeaponIcon;
        ViewData.BackWeaponName = FText::FromString(BackWeaponData.WeaponName);
		TArray<FKeywordDefinitionData> Keywords;
		DataManager->GetAllEnabledKeywordDefinitions(Keywords);
		const FWeaponNumericStats BaseStats{BackWeaponData.AttackPoint, BackWeaponData.BehaviorPoint, BackWeaponData.Count};
		ViewData.BackWeaponStats = {BaseStats.AttackPoint, BaseStats.WeaponPoint, BaseStats.WeaponCnt};
		ViewData.BackDescription = FCoinDescriptionFormatter::Parse(BackWeaponData.KOR_DES, Keywords, BaseStats);
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

void ABattlePlayerController_FlipSide::CreateSampleCoin(
	int32 FrontID,
	int32 BackID,
	int32 ReadyCoinSlotNum)
{
	UWorld* World = GetWorld();
	UCoinManagementWSubsystem* CoinManager = IsValid(World)
		? World->GetSubsystem<UCoinManagementWSubsystem>()
		: nullptr;
	if (!IsValid(CoinManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePlayerController] CreateSampleCoin 실패: CoinManager가 유효하지 않습니다."));
		return;
	}

	// 교체될 필드 코인을 관찰하던 델리게이트와 사거리 미리보기를 먼저 정리합니다.
	ClearReadySlotHighlight();
	ResetBattleInfoSelection();
	HideBattleCoinRangePreviews();
	HoveredBattleCoin.Reset();
	StopObservingBattleInfoCoin();

	if (!CoinManager->ReplaceReadyCoinWithSample(FrontID, BackID, ReadyCoinSlotNum))
	{
		RefreshHoveredBattleCoinInfo();
		return;
	}

	RefreshBattleCoinHUD();
	RefreshHoveredBattleCoinInfo();
}

void ABattlePlayerController_FlipSide::DoCoinDeath(int32 CoinReadySlotNum)
{
	UWorld* World = GetWorld();
	UCoinManagementWSubsystem* CoinManager = IsValid(World)
		? World->GetSubsystem<UCoinManagementWSubsystem>()
		: nullptr;
	if (!IsValid(CoinManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePlayerController] DoCoinDeath 실패: CoinManager가 유효하지 않습니다."));
		return;
	}

	const int32 ReadySlotCount = CoinManager->GetReadyCoinData().Num();
	if (CoinReadySlotNum <= 0 || CoinReadySlotNum > ReadySlotCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePlayerController] DoCoinDeath 실패: 준비 슬롯 번호는 1~%d이어야 합니다."), ReadySlotCount);
		return;
	}

	ACoinActor* Coin = CoinManager->GetRuntimeCoinAtReadySlot(CoinReadySlotNum - 1);
	UComponent_Status* Status = IsValid(Coin) ? Coin->StatComponent : nullptr;
	if (!IsValid(Status) || Status->IsDead())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattlePlayerController] DoCoinDeath 실패: 준비 슬롯 %d에 살아 있는 필드 코인이 없습니다."), CoinReadySlotNum);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BattlePlayerController] DoCoinDeath: 준비 슬롯 %d의 코인을 죽입니다."), CoinReadySlotNum);
	Status->SetHP(0, false);
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
