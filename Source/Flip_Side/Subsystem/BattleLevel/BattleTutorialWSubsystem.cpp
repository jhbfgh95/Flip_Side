#include "Subsystem/BattleLevel/BattleTutorialWSubsystem.h"
#include "Actors/TutorialTargetPoint.h"
#include "Actors/CoinActor.h"
#include "BattleTutorialSequenceData.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Player/BattlePlayerController_FlipSide.h"
#include "UI/BattlePlayerHUDWidget.h"
#include "UI/W_BattleTutorialOverlay.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "Subsystem/BattleLevel/CoinActionManagementWSubsystem.h"
#include "Subsystem/BattleLevel/UseableItemWSubsystem.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	UButton* FindButton(UWidget* Widget)
	{
		if (!IsValid(Widget)) return nullptr;
		if (UButton* Button = Cast<UButton>(Widget)) return Button;
		if (UUserWidget* User = Cast<UUserWidget>(Widget))
			if (User->WidgetTree) return FindButton(User->WidgetTree->RootWidget);
		if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
			for (int32 Index = 0; Index < Panel->GetChildrenCount(); ++Index)
				if (UButton* Button = FindButton(Panel->GetChildAt(Index))) return Button;
		return nullptr;
	}
}

bool UBattleTutorialWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return Super::ShouldCreateSubsystem(Outer) && IsValid(World) &&
		World->GetName().Contains(TEXT("L_Stage_BattleTutorial"));
}

void UBattleTutorialWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	const ULevelGISubsystem* Level = InWorld.GetGameInstance()->GetSubsystem<ULevelGISubsystem>();
	if (!IsValid(Level) || !Level->IsBattleTutorialActive()) return;
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	InWorld.GetTimerManager().SetTimer(InitBattleTutorialTimerHandle, this,
		&UBattleTutorialWSubsystem::InitBattleTutorialFromSettings, FMath::Max(0.01f, Settings->BattleTutorialInitDelay), false);
}

void UBattleTutorialWSubsystem::InitBattleTutorialFromSettings()
{
	BattlePlayerController = Cast<ABattlePlayerController_FlipSide>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(BattlePlayerController) || !IsValid(BattlePlayerController->GetBattleHUDWidget()))
	{
		GetWorld()->GetTimerManager().SetTimer(InitBattleTutorialTimerHandle, this,
			&UBattleTutorialWSubsystem::InitBattleTutorialFromSettings, 0.1f, false);
		return;
	}
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	InitBattleTutorial(Settings->BattleTutorialSequenceData.LoadSynchronous(), Settings->BattleTutorialOverlayWidgetClass.LoadSynchronous());
}

void UBattleTutorialWSubsystem::InitBattleTutorial(UBattleTutorialSequenceData* InSequenceData,
	TSubclassOf<UW_BattleTutorialOverlay> InOverlayClass, int32 ZOrder)
{
	const ULevelGISubsystem* Level = GetWorld()->GetGameInstance()->GetSubsystem<ULevelGISubsystem>();
	if (!IsValid(Level) || !Level->IsBattleTutorialActive() || !IsValid(InSequenceData) || !InOverlayClass) return;
	EndBattleTutorial();
	SequenceData = InSequenceData;
	BattlePlayerController = Cast<ABattlePlayerController_FlipSide>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(BattlePlayerController)) return;
	CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
	BattleManager = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
	OverlayWidget = CreateWidget<UW_BattleTutorialOverlay>(BattlePlayerController, InOverlayClass);
	if (!IsValid(OverlayWidget)) return;
	bInitialized = true;
	CurrentStepIndex = 0;
	OverlayWidget->OnBattleTutorialOverlayClicked.AddUObject(this, &UBattleTutorialWSubsystem::HandleOverlayClicked);
	OverlayWidget->AddToViewport(ZOrder);
	CacheTutorialTargets();
	BindBattleEvents();
	ApplyCurrentStep();
}

void UBattleTutorialWSubsystem::Deinitialize()
{
	EndBattleTutorial();
	Super::Deinitialize();
}

void UBattleTutorialWSubsystem::EndBattleTutorial()
{
	if (IsValid(GetWorld())) GetWorld()->GetTimerManager().ClearTimer(InitBattleTutorialTimerHandle);
	UnbindBattleEvents();
	if (IsValid(OverlayWidget))
	{
		OverlayWidget->OnBattleTutorialOverlayClicked.RemoveAll(this);
		OverlayWidget->RemoveFromParent();
	}
	if (IsValid(BattlePlayerController)) BattlePlayerController->SetInputForTutorial(false);
	OverlayWidget = nullptr; SequenceData = nullptr; CoinManager = nullptr; BattleManager = nullptr; BattlePlayerController = nullptr;
	TutorialTargetMap.Empty(); CurrentStepIndex = INDEX_NONE; CurrentStepClickCount = 0;
	bInitialized = false; bAdvanceQueued = false; bWaitingForLanding = false;
}

void UBattleTutorialWSubsystem::FinishBattleTutorial()
{
	ULevelGISubsystem* Level = GetWorld()->GetGameInstance()->GetSubsystem<ULevelGISubsystem>();
	EndBattleTutorial();
	if (IsValid(Level)) Level->MovingTutorialLevel(2);
}

void UBattleTutorialWSubsystem::AdvanceBattleTutorial()
{
	if (!bInitialized || !IsValid(SequenceData)) return;
	++CurrentStepIndex;
	if (!SequenceData->Steps.IsValidIndex(CurrentStepIndex)) { FinishBattleTutorial(); return; }
	ApplyCurrentStep();
}

void UBattleTutorialWSubsystem::QueueAdvance()
{
	if (bAdvanceQueued || !bInitialized) return;
	bAdvanceQueued = true;
	const int32 Step = CurrentStepIndex;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, Step]()
	{
		bAdvanceQueued = false;
		if (!bInitialized || CurrentStepIndex != Step) return;
		if (SequenceData->Steps[Step].AdvanceType == EBattleTutorialAdvanceType::End) FinishBattleTutorial();
		else AdvanceBattleTutorial();
	}));
}

void UBattleTutorialWSubsystem::CacheTutorialTargets()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATutorialTargetPoint::StaticClass(), Actors);
	for (AActor* Actor : Actors)
		if (ATutorialTargetPoint* Point = Cast<ATutorialTargetPoint>(Actor); IsValid(Point) && !Point->FocusId.IsNone())
			TutorialTargetMap.Add(Point->FocusId, Point);
}

void UBattleTutorialWSubsystem::BindBattleEvents()
{
	if (IsValid(CoinManager)) CoinManager->OnTutorialReadyCoinAdded.AddUObject(this, &UBattleTutorialWSubsystem::HandleCoinSlotAdded);
	if (IsValid(BattleManager)) BattleManager->OnBattleTutorialLeverTriggered.AddUObject(this, &UBattleTutorialWSubsystem::HandleLeverTriggered);
	if (auto* Action = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>(); IsValid(Action))
	{
		Action->OnTutorialCoinActionCompleted.AddUObject(this, &UBattleTutorialWSubsystem::HandleCoinActionCompleted);
		Action->OnTutorialCoinActionStarted.AddUObject(this, &UBattleTutorialWSubsystem::HandleCoinActionStarted);
	}
	if (auto* Items = GetWorld()->GetSubsystem<UUseableItemWSubsystem>(); IsValid(Items))
	{
		Items->OnTutorialItemSelected.AddUObject(this, &UBattleTutorialWSubsystem::HandleItemSelected);
		Items->OnTutorialItemUsed.AddUObject(this, &UBattleTutorialWSubsystem::HandleItemUsed);
	}
}

void UBattleTutorialWSubsystem::UnbindBattleEvents()
{
	if (UButton* Button = ActionButton.Get()) Button->OnClicked.RemoveDynamic(this, &UBattleTutorialWSubsystem::HandleActionButtonClicked);
	ActionButton.Reset();
	if (IsValid(CoinManager)) CoinManager->OnTutorialReadyCoinAdded.RemoveAll(this);
	if (IsValid(BattleManager)) BattleManager->OnBattleTutorialLeverTriggered.RemoveAll(this);
	if (!IsValid(GetWorld())) return;
	if (auto* Action = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>(); IsValid(Action))
	{
		Action->OnTutorialCoinActionCompleted.RemoveAll(this);
		Action->OnTutorialCoinActionStarted.RemoveAll(this);
	}
	if (auto* Items = GetWorld()->GetSubsystem<UUseableItemWSubsystem>(); IsValid(Items))
	{
		Items->OnTutorialItemSelected.RemoveAll(this); Items->OnTutorialItemUsed.RemoveAll(this);
	}
}

UWidget* UBattleTutorialWSubsystem::ResolveWidget(const FString& Path) const
{
	UBattlePlayerHUDWidget* HUD = IsValid(BattlePlayerController) ? BattlePlayerController->GetBattleHUDWidget() : nullptr;
	if (!IsValid(HUD) || Path.IsEmpty()) return nullptr;
	TArray<FString> Parts; Path.ParseIntoArray(Parts, TEXT("/"), true);
	UWidget* Current = HUD;
	for (int32 Index = 0; Index < Parts.Num(); ++Index)
	{
		if (Index == 0 && Parts[Index].StartsWith(TEXT("CoinSlot[")))
		{
			Current = HUD->GetTutorialCoinSlot(FCString::Atoi(*Parts[Index].Mid(9)));
			continue;
		}
		if (UUserWidget* User = Cast<UUserWidget>(Current))
			Current = User->WidgetTree ? User->WidgetTree->FindWidget(FName(*Parts[Index])) : nullptr;
		else if (UPanelWidget* Panel = Cast<UPanelWidget>(Current))
		{
			Current = nullptr;
			for (int32 Child = 0; Child < Panel->GetChildrenCount(); ++Child)
				if (Panel->GetChildAt(Child)->GetName() == Parts[Index]) { Current = Panel->GetChildAt(Child); break; }
		}
		else return nullptr;
		if (!IsValid(Current)) return nullptr;
	}
	return Current;
}

ACoinActor* UBattleTutorialWSubsystem::ResolveRuntimeCoin(int32 Index) const
{
	if (!IsValid(CoinManager) || Index < 0 || Index > 3) return nullptr;
	int32 Ordinal = 0;
	const auto& Ready = CoinManager->GetReadyCoinData();
	for (int32 Slot = 0; Slot < Ready.Num(); ++Slot)
		if (Ready[Slot].CoinInstanceID != INDEX_NONE && Ready[Slot].SourceSlotNumber == (Index < 2 ? 1 : 2))
		{
			if (Ordinal == Index % 2) return CoinManager->GetRuntimeCoinAtReadySlot(Slot);
			++Ordinal;
		}
	return nullptr;
}

void UBattleTutorialWSubsystem::ApplyCurrentStep()
{
	if (!IsValid(SequenceData) || !IsValid(OverlayWidget) || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{ EndBattleTutorial(); return; }
	if (UButton* Button = ActionButton.Get()) Button->OnClicked.RemoveDynamic(this, &UBattleTutorialWSubsystem::HandleActionButtonClicked);
	ActionButton.Reset(); CurrentStepClickCount = 0; ReadyCountAtStepStart = 0;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (IsValid(CoinManager))
		for (const FReadyCoinData& Coin : CoinManager->GetReadyCoinData())
			if (Coin.CoinInstanceID != INDEX_NONE && (Step.CoinSlotNumber == 0 || Coin.SourceSlotNumber == Step.CoinSlotNumber))
				++ReadyCountAtStepStart;
	OverlayWidget->ShowStep(Step);
	OverlayWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	// 바깥 영역 입력은 오버레이에서 소비하고 구멍 안의 실제 UI/월드 입력은 그대로 전달합니다.
	BattlePlayerController->SetInputForTutorial(false);
	RefreshTarget();
}

void UBattleTutorialWSubsystem::RefreshTarget()
{
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (Step.TargetType == EBattleTutorialTargetType::Actor)
	{
		AActor* Target = ResolveRuntimeCoin(Step.RuntimeCoinIndex);
		if (!IsValid(Target))
			if (const auto* Found = TutorialTargetMap.Find(Step.FocusId)) Target = Found->Get();
		OverlayWidget->SetActorTarget(Target, Step.HoleSize.IsNearlyZero() ? FVector2D(0.08f, 0.14f) : Step.HoleSize);
	}
	else if (Step.TargetType == EBattleTutorialTargetType::Widget)
	{
		FString Path = Step.WidgetPath;
		if (Path.IsEmpty())
		{
			if (Step.FocusId == TEXT("Lever")) Path = TEXT("LeverWidget");
			else if (Step.FocusId == TEXT("CoinSlot")) Path = FString::Printf(TEXT("CoinSlot[%d]"), FMath::Max(1, Step.CoinSlotNumber));
			else if (Step.FocusId == TEXT("Card")) Path = TEXT("CardSlot1");
			else if (Step.FocusId == TEXT("ItemSlot")) Path = TEXT("ItemSlot1");
			else if (Step.FocusId == TEXT("Drawer") || Step.FocusId == TEXT("ReadyCoin")) Path = TEXT("BattleReadyCoinWidget");
		}
		UWidget* Target = ResolveWidget(Path);
		OverlayWidget->SetWidgetTarget(Target);
		if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::TargetAction)
		{
			UButton* Button = FindButton(Step.ActionWidgetPath.IsEmpty() ? Target : ResolveWidget(Step.ActionWidgetPath));
			if (Button != ActionButton.Get())
			{
				if (UButton* Old = ActionButton.Get()) Old->OnClicked.RemoveDynamic(this, &UBattleTutorialWSubsystem::HandleActionButtonClicked);
				ActionButton = Button;
				if (IsValid(Button)) Button->OnClicked.AddUniqueDynamic(this, &UBattleTutorialWSubsystem::HandleActionButtonClicked);
			}
		}
	}
}

void UBattleTutorialWSubsystem::HandleOverlayClicked()
{
	if (bInitialized && !SequenceData->Steps[CurrentStepIndex].bRequireHighlightedAction) QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleCoinSlotAdded(int32 SlotNumber)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (!Step.bRequireHighlightedAction || Step.AdvanceType != EBattleTutorialAdvanceType::CoinSlotClick ||
		(Step.CoinSlotNumber != 0 && Step.CoinSlotNumber != SlotNumber)) return;
	int32 Count = 0;
	for (const FReadyCoinData& Coin : CoinManager->GetReadyCoinData())
		if (Coin.CoinInstanceID != INDEX_NONE && (Step.CoinSlotNumber == 0 || Coin.SourceSlotNumber == Step.CoinSlotNumber)) ++Count;
	if (Count - ReadyCountAtStepStart >= Step.RequiredClickCount) QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleLeverTriggered()
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (!Step.bRequireHighlightedAction || Step.AdvanceType != EBattleTutorialAdvanceType::LeverClick) return;
	bWaitingForLanding = true;
	OverlayWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UBattleTutorialWSubsystem::HandleCoinActionCompleted(ACoinActor* Coin)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::CoinAction &&
		IsValid(Coin) && Coin == ResolveRuntimeCoin(Step.ActionRuntimeCoinIndex >= 0 ? Step.ActionRuntimeCoinIndex : Step.RuntimeCoinIndex)) QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleCoinActionStarted(ACoinActor* Coin)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::CoinActionStarted &&
		IsValid(Coin) && Coin == ResolveRuntimeCoin(Step.ActionRuntimeCoinIndex >= 0 ? Step.ActionRuntimeCoinIndex : Step.RuntimeCoinIndex)) QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleItemSelected(int32 ItemID)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::ItemSelected &&
		(Step.ItemID < 0 || Step.ItemID == ItemID)) QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleItemUsed(int32 ItemID)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::ItemUsed &&
		(Step.ItemID < 0 || Step.ItemID == ItemID)) QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleActionButtonClicked()
{
	if (++CurrentStepClickCount >= SequenceData->Steps[CurrentStepIndex].RequiredClickCount) QueueAdvance();
}

void UBattleTutorialWSubsystem::NotifyTargetAction(FName ActionId)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && !ActionId.IsNone() && Step.ActionId == ActionId) HandleActionButtonClicked();
}

bool UBattleTutorialWSubsystem::CanAddTutorialCoin(int32 SlotNumber) const
{
	if (!bInitialized || !SequenceData->Steps.IsValidIndex(CurrentStepIndex)) return false;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	return Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::CoinSlotClick &&
		(Step.CoinSlotNumber == 0 || Step.CoinSlotNumber == SlotNumber);
}

bool UBattleTutorialWSubsystem::CanActWithTutorialCoin(const ACoinActor* Coin) const
{
	if (!bInitialized || !SequenceData->Steps.IsValidIndex(CurrentStepIndex)) return false;
	const FBattleTutorialStep& Step = SequenceData->Steps[CurrentStepIndex];
	return Step.bRequireHighlightedAction && (Step.AdvanceType == EBattleTutorialAdvanceType::CoinAction ||
		Step.AdvanceType == EBattleTutorialAdvanceType::CoinActionStarted) &&
		Coin == ResolveRuntimeCoin(Step.ActionRuntimeCoinIndex >= 0 ? Step.ActionRuntimeCoinIndex : Step.RuntimeCoinIndex);
}

bool UBattleTutorialWSubsystem::CanProgressTutorialPhase() const
{
	return bInitialized && !bWaitingForLanding && !bAdvanceQueued && SequenceData->Steps.IsValidIndex(CurrentStepIndex) &&
		SequenceData->Steps[CurrentStepIndex].bRequireHighlightedAction &&
		SequenceData->Steps[CurrentStepIndex].AdvanceType == EBattleTutorialAdvanceType::LeverClick;
}

void UBattleTutorialWSubsystem::Tick(float DeltaTime)
{
	if (!bInitialized) return;
	if (bWaitingForLanding)
	{
		const auto* Items = GetWorld()->GetSubsystem<UUseableItemWSubsystem>();
		if (BattleManager->GetCurrentPhase() != EPhaseState::CoinBehaviorPhase || (IsValid(Items) && Items->IsItemUseAvailable()))
		{ bWaitingForLanding = false; QueueAdvance(); }
		return;
	}
	RefreshTarget();
}

TStatId UBattleTutorialWSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBattleTutorialWSubsystem, STATGROUP_Tickables);
}
