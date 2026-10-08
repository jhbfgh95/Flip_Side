#include "Subsystem/BattleLevel/BattleTutorialWSubsystem.h"
#include "Actors/TutorialTargetPoint.h"
#include "Actors/CoinActor.h"
#include "Actors/Component_Status.h"
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
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/BossManagerSubsystem.h"
#include "Subsystem/BattleLevel/BattleLevelActingWSubsystem.h"
#include "Actors/GridActor.h"
#include "Actors/Boss/BossActor.h"
#include "Actors/Boss/BossPillarActor.h"
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

void UBattleTutorialWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UBattleManagerWSubsystem>();
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
	TArray<FBattleTutorialStep> OrderedSteps = InSequenceData->GetOrderedSteps();
	if (OrderedSteps.IsEmpty()) return;
	EndBattleTutorial();
	SequenceData = InSequenceData;
	TutorialSteps = MoveTemp(OrderedSteps);
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
	if (IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().ClearTimer(InitBattleTutorialTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(AdvanceTimerHandle);
	}
	UnbindBattleEvents();
	if (IsValid(OverlayWidget))
	{
		OverlayWidget->OnBattleTutorialOverlayClicked.RemoveAll(this);
		OverlayWidget->RemoveFromParent();
	}
	if (IsValid(BattlePlayerController))
	{
		BattlePlayerController->SetTutorialRangePreviewCoin(nullptr);
		BattlePlayerController->SetInputForTutorial(false);
	}
	if (IsValid(BattlePlayerController) && IsValid(BattlePlayerController->GetBattleHUDWidget()))
		BattlePlayerController->GetBattleHUDWidget()->SetTutorialInfoPinned(false);
	OverlayWidget = nullptr; SequenceData = nullptr; CoinManager = nullptr; BattleManager = nullptr; BattlePlayerController = nullptr;
	TutorialSteps.Reset();
	TutorialTargetMap.Empty(); CurrentStepIndex = INDEX_NONE; CurrentStepClickCount = 0;
	bInitialized = false; bAdvanceQueued = false; bWaitingForLanding = false;
	bWaitingForAbilitySelection = false; bWaitingForItemAnimation = false; bWaitingForBossPattern = false;
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
	if (IsValid(GetWorld())) GetWorld()->GetTimerManager().ClearTimer(AdvanceTimerHandle);
	bAdvanceQueued = false;
	ApplyStepExit();
	++CurrentStepIndex;
	if (!TutorialSteps.IsValidIndex(CurrentStepIndex)) { FinishBattleTutorial(); return; }
	ApplyCurrentStep();
}

void UBattleTutorialWSubsystem::QueueAdvance()
{
	UWorld* World = GetWorld();
	if (bAdvanceQueued || !bInitialized || !IsValid(World) || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return;
	bAdvanceQueued = true;
	if (IsValid(OverlayWidget)) OverlayWidget->SetAdvancePending(true);
	const int32 Step = CurrentStepIndex;
	const FTimerDelegate AdvanceDelegate = FTimerDelegate::CreateWeakLambda(this, [this, Step]()
	{
		if (!bInitialized || !bAdvanceQueued || CurrentStepIndex != Step || !TutorialSteps.IsValidIndex(Step)) return;
		bAdvanceQueued = false;
		if (TutorialSteps[Step].AdvanceType == EBattleTutorialAdvanceType::End) FinishBattleTutorial();
		else AdvanceBattleTutorial();
	});
	const float Delay = TutorialSteps[Step].NextStepDelay;
	if (Delay > 0.f) World->GetTimerManager().SetTimer(AdvanceTimerHandle, AdvanceDelegate, Delay, false);
	else AdvanceTimerHandle = World->GetTimerManager().SetTimerForNextTick(AdvanceDelegate);
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
	if (IsValid(BattleManager))
	{
		BattleManager->OnBattleTutorialLeverTriggered.AddUObject(this, &UBattleTutorialWSubsystem::HandleLeverTriggered);
		BattleManager->OnBossPhaseCompleted.AddUniqueDynamic(this, &UBattleTutorialWSubsystem::HandleBossPatternFinished);
	}
	if (auto* Action = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>(); IsValid(Action))
	{
		Action->OnTutorialCoinActionCompleted.AddUObject(this, &UBattleTutorialWSubsystem::HandleCoinActionCompleted);
		Action->OnTutorialCoinActionStarted.AddUObject(this, &UBattleTutorialWSubsystem::HandleCoinActionStarted);
		Action->OnTutorialCoinActionFinished.AddUObject(this, &UBattleTutorialWSubsystem::HandleCoinActionFinished);
	}
	if (auto* Items = GetWorld()->GetSubsystem<UUseableItemWSubsystem>(); IsValid(Items))
	{
		Items->OnTutorialItemSelected.AddUObject(this, &UBattleTutorialWSubsystem::HandleItemSelected);
		Items->OnTutorialItemUsed.AddUObject(this, &UBattleTutorialWSubsystem::HandleItemUsed);
	}
	if (auto* Acting = GetWorld()->GetSubsystem<UBattleLevelActingWSubsystem>(); IsValid(Acting))
		Acting->OnTutorialItemAnimationFinished.AddUObject(this, &UBattleTutorialWSubsystem::HandleItemAnimationFinished);
}

void UBattleTutorialWSubsystem::UnbindBattleEvents()
{
	if (UButton* Button = ActionButton.Get()) Button->OnClicked.RemoveDynamic(this, &UBattleTutorialWSubsystem::HandleActionButtonClicked);
	ActionButton.Reset();
	if (IsValid(CoinManager)) CoinManager->OnTutorialReadyCoinAdded.RemoveAll(this);
	if (IsValid(BattleManager))
	{
		BattleManager->OnBattleTutorialLeverTriggered.RemoveAll(this);
		BattleManager->OnBossPhaseCompleted.RemoveDynamic(this, &UBattleTutorialWSubsystem::HandleBossPatternFinished);
	}
	if (!IsValid(GetWorld())) return;
	if (auto* Action = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>(); IsValid(Action))
	{
		Action->OnTutorialCoinActionCompleted.RemoveAll(this);
		Action->OnTutorialCoinActionStarted.RemoveAll(this);
		Action->OnTutorialCoinActionFinished.RemoveAll(this);
	}
	if (auto* Items = GetWorld()->GetSubsystem<UUseableItemWSubsystem>(); IsValid(Items))
	{
		Items->OnTutorialItemSelected.RemoveAll(this); Items->OnTutorialItemUsed.RemoveAll(this);
	}
	if (auto* Acting = GetWorld()->GetSubsystem<UBattleLevelActingWSubsystem>(); IsValid(Acting))
		Acting->OnTutorialItemAnimationFinished.RemoveAll(this);
}

UWidget* UBattleTutorialWSubsystem::ResolveWidget(const FString& Path) const
{
	UBattlePlayerHUDWidget* HUD = IsValid(BattlePlayerController) ? BattlePlayerController->GetBattleHUDWidget() : nullptr;
	if (!IsValid(HUD) || Path.IsEmpty()) return nullptr;
	TArray<FString> Parts; Path.ParseIntoArray(Parts, TEXT("/"), true);
	UWidget* Current = HUD;
	for (int32 Index = 0; Index < Parts.Num(); ++Index)
	{
		if (Index == 0 && Parts[Index] == TEXT("@CoinSlotInfo")) { Current = HUD->GetTutorialCoinInfo(); continue; }
		if (Index == 0 && Parts[Index] == TEXT("@ItemInfo")) { Current = HUD->GetTutorialItemInfo(); continue; }
		if (Index == 0 && Parts[Index] == TEXT("@CardInfo")) { Current = HUD->GetTutorialCardInfo(); continue; }
		if (Index == 0 && Parts[Index] == TEXT("@BattleCoinMobilityBookmark")) { Current = HUD->GetTutorialMobilityBookmark(); continue; }
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
	if (!IsValid(SequenceData) || !IsValid(OverlayWidget) || !TutorialSteps.IsValidIndex(CurrentStepIndex))
	{ EndBattleTutorial(); return; }
	if (UButton* Button = ActionButton.Get()) Button->OnClicked.RemoveDynamic(this, &UBattleTutorialWSubsystem::HandleActionButtonClicked);
	ActionButton.Reset(); CurrentStepClickCount = 0; ReadyCountAtStepStart = 0;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.TargetType == EBattleTutorialTargetType::Widget && Step.ActionId.ToString().StartsWith(TEXT("Hover")))
		if (UBattlePlayerHUDWidget* HUD = BattlePlayerController->GetBattleHUDWidget(); IsValid(HUD)) HUD->SetTutorialInfoPinned(true);
	const bool bAbilityRange = Step.FocusId == TEXT("AbilityRange");
	BattlePlayerController->SetTutorialRangePreviewCoin(Step.FocusId == TEXT("AttackRangeAndBoss") || bAbilityRange
		? ResolveRuntimeCoin(bAbilityRange && Step.ActionRuntimeCoinIndex >= 0 ? Step.ActionRuntimeCoinIndex : Step.RuntimeCoinIndex)
		: nullptr, bAbilityRange);
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
	if (!IsValid(OverlayWidget) || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.TargetType == EBattleTutorialTargetType::Actor)
	{
		const bool bShowFieldAnimation = bWaitingForItemAnimation ||
			((bWaitingForAbilitySelection || bAdvanceQueued) && Step.FocusId == TEXT("FieldOnAction"));
		if (Step.FocusId == TEXT("FieldArea") || Step.FocusId == TEXT("BossArea") || bShowFieldAnimation)
		{
			TArray<AActor*> Targets;
			const auto* Grid = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
			if (IsValid(Grid))
				for (int32 Y = Step.FocusId == TEXT("BossArea") && !bShowFieldAnimation ? Grid->GetBossAreaStartY() : 0; Y < Grid->GridYSize; ++Y)
					for (int32 X = 0; X < Grid->GridXSize; ++X)
						if (AGridActor* Cell = Grid->GetGridActor(FGridPoint(X, Y)); IsValid(Cell)) Targets.Add(Cell);
			if (const auto* Boss = GetWorld()->GetSubsystem<UBossManagerSubsystem>(); IsValid(Boss) && IsValid(Boss->GetCurrentBoss())) Targets.Add(Boss->GetCurrentBoss());
			TArray<AActor*> Pillars; UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossPillarActor::StaticClass(), Pillars); Targets.Append(Pillars);
			if (Step.FocusId == TEXT("FieldArea") || bShowFieldAnimation)
				for (int32 Index = 0; Index < 4; ++Index) if (ACoinActor* Coin = ResolveRuntimeCoin(Index); IsValid(Coin)) Targets.Add(Coin);
			OverlayWidget->SetActorTargets(Targets); return;
		}
		if (Step.FocusId == TEXT("CoinPair"))
		{
			TArray<AActor*> Targets;
			if (ACoinActor* Coin = ResolveRuntimeCoin(Step.RuntimeCoinIndex); IsValid(Coin)) Targets.Add(Coin);
			if (ACoinActor* Coin = ResolveRuntimeCoin(Step.ActionRuntimeCoinIndex); IsValid(Coin)) Targets.AddUnique(Coin);
			OverlayWidget->SetActorTargets(Targets); return;
		}
		if (Step.FocusId == TEXT("AttackRangeAndBoss") || Step.FocusId == TEXT("AbilityRange"))
		{
			TArray<AActor*> Targets;
			TArray<FGridPoint> Cells;
			const bool bAbilityRange = Step.FocusId == TEXT("AbilityRange");
			ACoinActor* Coin = ResolveRuntimeCoin(bAbilityRange && Step.ActionRuntimeCoinIndex >= 0
				? Step.ActionRuntimeCoinIndex : Step.RuntimeCoinIndex);
			const auto* Grid = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
			if (IsValid(Coin)) Targets.Add(Coin);
			bool bHasActiveRange = false;
			if (bAbilityRange)
				if (const auto* Action = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>(); IsValid(Action))
					bHasActiveRange = Action->GetActiveAbilityPreviewCells(Cells);
			if (!bHasActiveRange && IsValid(Grid) && IsValid(Coin) && IsValid(Coin->StatComponent))
			{
				const FWeaponActionSnapshot Snapshot = Coin->StatComponent->BuildActionSnapshot(Coin->GetCoinDecidedFace());
				if (bAbilityRange && Snapshot.bHasAbilityArea)
					Grid->BuildAbilityAreaCellsFromOrigin(Coin->GetDecidedGrid(), Snapshot.AbilityAreaSpec, Cells);
				else if (!bAbilityRange)
					Grid->BuildAreaCellsFromOrigin(Coin->GetDecidedGrid(), Snapshot.AttackAreaSpec, Cells);
			}
			if (IsValid(Grid))
				for (const FGridPoint& Cell : Cells)
					if (AGridActor* Actor = Grid->GetGridActor(Cell); IsValid(Actor)) Targets.AddUnique(Actor);
			if (bAbilityRange)
			{
				if (ACoinActor* Target = ResolveRuntimeCoin(Step.RuntimeCoinIndex); IsValid(Target)) Targets.AddUnique(Target);
			}
			else
			{
				if (const auto* Boss = GetWorld()->GetSubsystem<UBossManagerSubsystem>(); IsValid(Boss) && IsValid(Boss->GetCurrentBoss()))
					Targets.Add(Boss->GetCurrentBoss());
				TArray<AActor*> Pillars; UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossPillarActor::StaticClass(), Pillars); Targets.Append(Pillars);
			}
			OverlayWidget->SetActorTargets(Targets); return;
		}
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
		if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::TargetAction &&
			!Step.ActionId.ToString().StartsWith(TEXT("Hover")))
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
	if (!bInitialized || bAdvanceQueued || bWaitingForBossPattern || bWaitingForAbilitySelection || bWaitingForItemAnimation) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction) return;
	if (Step.bRunBossPatternOnExit)
	{
		bWaitingForBossPattern = true; OverlayWidget->SetTransitionBusy(true);
		BattleManager->ResumeTutorialBossPattern();
	}
	else QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleCoinSlotAdded(int32 SlotNumber)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
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
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (!Step.bRequireHighlightedAction || Step.AdvanceType != EBattleTutorialAdvanceType::LeverClick) return;
	bWaitingForLanding = true;
	OverlayWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UBattleTutorialWSubsystem::HandleCoinActionCompleted(ACoinActor* Coin)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::CoinAction &&
		IsValid(Coin) && Coin == ResolveRuntimeCoin(Step.ActionRuntimeCoinIndex >= 0 ? Step.ActionRuntimeCoinIndex : Step.RuntimeCoinIndex)) QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleCoinActionStarted(ACoinActor* Coin)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::CoinActionStarted &&
		IsValid(Coin) && Coin == ResolveRuntimeCoin(Step.ActionRuntimeCoinIndex >= 0 ? Step.ActionRuntimeCoinIndex : Step.RuntimeCoinIndex))
	{
		bWaitingForAbilitySelection = true;
		OverlayWidget->SetTransitionBusy(true);
		RefreshTarget();
	}
	if (Step.FocusId == TEXT("AttackRangeAndBoss") && IsValid(BattlePlayerController))
		BattlePlayerController->SetTutorialRangePreviewCoin(nullptr);
}

void UBattleTutorialWSubsystem::HandleCoinActionFinished(ACoinActor* Coin)
{
	if (!bInitialized || !IsValid(Coin)) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Coin == ResolveRuntimeCoin(Step.RuntimeCoinIndex) && Step.ActionId == TEXT("PipeFailureFinished"))
		NotifyTargetAction(Step.ActionId);
}

void UBattleTutorialWSubsystem::HandleItemAnimationFinished()
{
	bItemAnimationFinished = true;
	if (bInitialized && bWaitingForItemAnimation) { bWaitingForItemAnimation = false; QueueAdvance(); }
}

void UBattleTutorialWSubsystem::HandleBossPatternFinished()
{
	if (bInitialized && bWaitingForBossPattern) { bWaitingForBossPattern = false; QueueAdvance(); }
}

void UBattleTutorialWSubsystem::ApplyStepExit()
{
	if (!IsValid(BattlePlayerController) || !IsValid(SequenceData) || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return;
	UBattlePlayerHUDWidget* HUD = BattlePlayerController->GetBattleHUDWidget(); if (!IsValid(HUD)) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.WidgetPath == TEXT("@CardInfo"))
	{
		HUD->SetTutorialInfoPinned(false);
		HUD->DismissTutorialCardInfo();
	}
	if (Step.bCloseCoinInfoOnExit || Step.bCloseItemInfoOnExit || Step.bReturnToReadyOnExit) HUD->SetTutorialInfoPinned(false);
	if (Step.bCloseCoinInfoOnExit) HUD->DismissCoinSlotInfo();
	if (Step.bCloseItemInfoOnExit) HUD->DismissItemInfo();
	if (Step.bReturnToReadyOnExit) HUD->ReturnTutorialInfoToReady();
}

void UBattleTutorialWSubsystem::CheckHoverStep()
{
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (!Step.bRequireHighlightedAction || !Step.ActionId.ToString().StartsWith(TEXT("Hover"))) return;
	UBattlePlayerHUDWidget* HUD = BattlePlayerController->GetBattleHUDWidget(); if (!IsValid(HUD)) return;
	if (Step.TargetType == EBattleTutorialTargetType::Widget && Step.ActionId == TEXT("HoverCard1"))
	{
		// 화면의 CardSlot1은 StageCard의 0번 슬롯입니다. 실제 Button 호버와 열린 정보창을 확인합니다.
		if (HUD->IsTutorialCardSlotHovered(0)) NotifyTargetAction(Step.ActionId);
		return;
	}
	if (Step.TargetType == EBattleTutorialTargetType::Actor)
	{
		ACoinActor* Coin = ResolveRuntimeCoin(Step.RuntimeCoinIndex);
		if (IsValid(Coin) && BattlePlayerController->GetTutorialHoveredCoin() == Coin) NotifyTargetAction(Step.ActionId);
	}
	else if (UWidget* Widget = ResolveWidget(Step.WidgetPath); IsValid(Widget) && Widget->IsHovered())
	{
		if ((Step.ActionId == TEXT("HoverCoinSlot1") && HUD->IsCoinSlotInfoOpen()) ||
			(Step.ActionId == TEXT("HoverItem4") && HUD->IsItemInfoOpen())) NotifyTargetAction(Step.ActionId);
	}
}

void UBattleTutorialWSubsystem::HandleItemSelected(int32 ItemID)
{
	bItemAnimationFinished = false;
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::ItemSelected &&
		(Step.ItemID < 0 || Step.ItemID == ItemID)) QueueAdvance();
}

void UBattleTutorialWSubsystem::HandleItemUsed(int32 ItemID)
{
	if (!bInitialized) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::ItemUsed &&
		(Step.ItemID < 0 || Step.ItemID == ItemID))
	{
		if (ItemID == 4 && !bItemAnimationFinished)
		{
			bWaitingForItemAnimation = true;
			OverlayWidget->SetTransitionBusy(true);
			RefreshTarget();
		}
		else QueueAdvance();
	}
}

void UBattleTutorialWSubsystem::HandleActionButtonClicked()
{
	if (!bInitialized || bAdvanceQueued || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return;
	if (++CurrentStepClickCount >= TutorialSteps[CurrentStepIndex].RequiredClickCount) QueueAdvance();
}

void UBattleTutorialWSubsystem::NotifyTargetAction(FName ActionId)
{
	if (!bInitialized || bAdvanceQueued) return;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	if (Step.bRequireHighlightedAction && !ActionId.IsNone() && Step.ActionId == ActionId) HandleActionButtonClicked();
}

bool UBattleTutorialWSubsystem::CanAddTutorialCoin(int32 SlotNumber) const
{
	if (!bInitialized || bAdvanceQueued || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return false;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	return Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::CoinSlotClick &&
		(Step.CoinSlotNumber == 0 || Step.CoinSlotNumber == SlotNumber);
}

bool UBattleTutorialWSubsystem::CanActWithTutorialCoin(const ACoinActor* Coin) const
{
	if (!bInitialized || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return false;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	return !bAdvanceQueued && Step.bRequireHighlightedAction && (Step.AdvanceType == EBattleTutorialAdvanceType::CoinAction ||
		Step.AdvanceType == EBattleTutorialAdvanceType::CoinActionStarted || Step.ActionId == TEXT("PipeFailureFinished")) &&
		Coin == ResolveRuntimeCoin(Step.ActionRuntimeCoinIndex >= 0 ? Step.ActionRuntimeCoinIndex : Step.RuntimeCoinIndex);
}

bool UBattleTutorialWSubsystem::CanSelectTutorialCoin(const ACoinActor* Coin) const
{
	if (!bInitialized || bAdvanceQueued || bWaitingForAbilitySelection || bWaitingForItemAnimation || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return false;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	return IsValid(Coin) && Coin == ResolveRuntimeCoin(Step.RuntimeCoinIndex) && Step.bRequireHighlightedAction &&
		(Step.AdvanceType == EBattleTutorialAdvanceType::CoinAction || Step.AdvanceType == EBattleTutorialAdvanceType::ItemUsed);
}

bool UBattleTutorialWSubsystem::CanSelectTutorialItem(int32 ItemID) const
{
	if (!bInitialized || bAdvanceQueued || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return false;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	return Step.bRequireHighlightedAction && Step.AdvanceType == EBattleTutorialAdvanceType::ItemSelected && (Step.ItemID < 0 || Step.ItemID == ItemID);
}

bool UBattleTutorialWSubsystem::CanHoverTutorialCoin(const ACoinActor* Coin) const
{
	if (!bInitialized || !TutorialSteps.IsValidIndex(CurrentStepIndex)) return false;
	const FBattleTutorialStep& Step = TutorialSteps[CurrentStepIndex];
	return Step.TargetType == EBattleTutorialTargetType::Actor && IsValid(Coin) && Coin == ResolveRuntimeCoin(Step.RuntimeCoinIndex);
}

bool UBattleTutorialWSubsystem::IsTutorialWorldClickAllowed() const
{
	return bInitialized && !bAdvanceQueued && !bWaitingForLanding && !bWaitingForAbilitySelection && !bWaitingForItemAnimation &&
		!bWaitingForBossPattern && TutorialSteps.IsValidIndex(CurrentStepIndex) && TutorialSteps[CurrentStepIndex].bRequireHighlightedAction;
}

bool UBattleTutorialWSubsystem::CanProgressTutorialPhase() const
{
	return bInitialized && !bWaitingForLanding && !bAdvanceQueued && TutorialSteps.IsValidIndex(CurrentStepIndex) &&
		TutorialSteps[CurrentStepIndex].bRequireHighlightedAction &&
		TutorialSteps[CurrentStepIndex].AdvanceType == EBattleTutorialAdvanceType::LeverClick;
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
	if (bWaitingForAbilitySelection)
	{
		const auto* Action = GetWorld()->GetSubsystem<UCoinActionManagementWSubsystem>();
		if (IsValid(Action) && (Action->CurrentInputState == EActionInputState::WaitingForCoinClick || !Action->IsActionSequenceActive()) &&
			Action->HaveTutorialActionVFXFinished())
		{ bWaitingForAbilitySelection = false; QueueAdvance(); }
		return;
	}
	if (!bAdvanceQueued && !bWaitingForItemAnimation && !bWaitingForBossPattern) CheckHoverStep();
}

TStatId UBattleTutorialWSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBattleTutorialWSubsystem, STATGROUP_Tickables);
}
