#include "UI/ShopTutorialPresenter.h"

#include "UI/ShopCard/ShopCardPresenter.h"
#include "UI/ShopCoinManage/ShopCoinPresenter.h"
#include "UI/ShopItem/ShopItemPresenter.h"
#include "UI/ShopPageChangePresenter.h"
#include "UI/ShopTutorialFlow.h"
#include "UI/ShopUnlockWeapon/UnlockWeaponPresenter.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponWidget.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlotContainer.h"
#include "UI/W_ShopNavigationBar.h"
#include "UI/W_ShopWidgetContainer.h"
#include "UI/W_ShopTutorialWidget.h"
#include "UI/ShopCoinManage/W_ShopCoinSlotContainer.h"
#include "UI/ShopCoinManage/W_ShopCoinSlot.h"
#include "UI/ShopCoinManage/W_ShopWeaponSlotContainer.h"
#include "UI/ShopCoinManage/ShopCoinUIActor.h"
#include "UI/ShopCard/W_ShopCardWidget.h"
#include "UI/ShopItem/W_ShopItemSlotContainer.h"
#include "Components/Widget.h"
#include "Components/Border.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Player/GameMode_Shop.h"
#include "UI/ShopCard/W_ShopPlayerCardSlotContainer.h"
#include "Components/Button.h"
#include "UI/ShopItem/W_ShopPlayerItemSlotContainer.h"
#include "UI/ShopUISelectRegistry.h"
#include "UI/ShopUISelectActor.h"

void UShopTutorialPresenter::InitShopActors(AShopUISelectRegistry* InShopUISelectRegistry, AShopCoinUIActor* InCoinUIActor)
{
	ShopUISelectRegistry = InShopUISelectRegistry;
	CoinUIActor = InCoinUIActor;
}

void UShopTutorialPresenter::InitPresenter(
	UShopTutorialWSubsystem* InTutorialSubsystem,
	UW_ShopTutorialWidget* InTutorialWidget,
	UW_ShopWidgetContainer* InShopWidgetContainer)
{
	TutorialSubsystem = InTutorialSubsystem;
	TutorialWidget = InTutorialWidget;
	ShopWidgetContainer = InShopWidgetContainer;
	NavigationBar = IsValid(ShopWidgetContainer)
		? ShopWidgetContainer->GetShopNavigationBar()
		: nullptr;
	if (IsValid(NavigationBar))
	{
		NavigationBar->OnNavigationToggleClicked.AddUniqueDynamic(
			this, &ThisClass::HandleNavigationToggleClicked);
	}

	if (!IsValid(TutorialSubsystem))
	{
		return;
	}

	TutorialSubsystem->OnTutorialStepChanged.AddUniqueDynamic(
		this, &ThisClass::HandleTutorialStepChanged);
	TutorialSubsystem->OnTutorialStarted.AddUniqueDynamic(
		this, &ThisClass::HandleTutorialStarted);
	TutorialSubsystem->OnTutorialCompleted.AddUniqueDynamic(
		this, &ThisClass::HandleTutorialCompleted);

	if (IsValid(TutorialWidget))
	{
		TutorialWidget->OnDialogueClicked.AddUniqueDynamic(this, &ThisClass::HandleDialogueClicked);
		TutorialWidget->SetHighlightOverlayVisible(false);
	}
}

void UShopTutorialPresenter::SetShopPresenters(
	UShopCardPresenter* InCardPresenter,
	UShopItemPresenter* InItemPresenter,
	UShopCoinPresenter* InCoinPresenter,
	UUnlockWeaponPresenter* InUnlockWeaponPresenter,
	UShopPageChangePresenter* InPageChangePresenter)
{
	CardPresenter = InCardPresenter;
	ItemPresenter = InItemPresenter;
	CoinPresenter = InCoinPresenter;
	UnlockWeaponPresenter = InUnlockWeaponPresenter;
	PageChangePresenter = InPageChangePresenter;
	ShopCoinWidget = IsValid(CoinPresenter) ? CoinPresenter->GetShopCoinWidget() : nullptr;
	ShopCardMainWidget = IsValid(CardPresenter) ? CardPresenter->GetShopCardMainWidget() : nullptr;
	ShopItemWidget = IsValid(ItemPresenter) ? ItemPresenter->GetShopItemWidget() : nullptr;

	if (IsValid(CardPresenter))
	{
		CardPresenter->OnCardPurchased.AddUniqueDynamic(this, &ThisClass::HandleCardPurchased);
		CardPresenter->OnPlayerCardSelected.AddUniqueDynamic(this, &ThisClass::HandlePlayerCardSelected);
	}

	if (IsValid(ItemPresenter))
	{
		ItemPresenter->OnItemPurchaseClicked.AddUniqueDynamic(this, &ThisClass::HandleItemPurchaseClicked);
		ItemPresenter->OnItemPurchased.AddUniqueDynamic(this, &ThisClass::HandleItemPurchased);
		ItemPresenter->OnItemSold.AddUniqueDynamic(this, &ThisClass::HandleItemSold);
	}

	if (IsValid(CoinPresenter))
	{
		CoinPresenter->OnCoinSlotPurchased.AddUniqueDynamic(this, &ThisClass::HandleCoinSlotPurchased);
		CoinPresenter->OnCoinWeaponClicked.AddUniqueDynamic(this, &ThisClass::HandleCoinWeaponClicked);
		CoinPresenter->OnCoinSideChanged.AddUniqueDynamic(this, &ThisClass::HandleCoinSideChanged);
		CoinPresenter->OnCoinCountIncreased.AddUniqueDynamic(this, &ThisClass::HandleCoinCountIncreased);
	}

	if (IsValid(UnlockWeaponPresenter))
	{
		UnlockWeaponPresenter->OnWeaponUnlocked.AddUniqueDynamic(this, &ThisClass::HandleWeaponUnlocked);
	}

	if (IsValid(PageChangePresenter))
	{
		PageChangePresenter->OnPageMoveCompleted.AddUniqueDynamic(
			this, &ThisClass::HandlePageMoveCompleted);
		PageChangePresenter->PageChangeStart.AddUniqueDynamic(
			this, &ThisClass::HandlePageChangeStart);
	}
	StartTutorial();
}

void UShopTutorialPresenter::StartTutorial()
{
	if (!IsValid(TutorialSubsystem))
	{
		return;
	}

	if (!IsValid(TutorialFlow))
	{
		TutorialFlow = NewObject<UShopTutorialFlow>(this);
		if (!IsValid(TutorialFlow))
		{
			return;
		}

		TutorialFlow->Init();
	}

	StartTutorialWithSequence(
		TutorialFlow->GetTutorialDialogueList(),
		TutorialFlow->GetTutorialActionSequence());
}

void UShopTutorialPresenter::StartTutorialWithSequence(
	const TArray<FText>& InDialogueList,
	const TArray<EShopTutorialAction>& InActionSequence)
{
	if (!IsValid(TutorialSubsystem))
	{
		return;
	}

	TutorialDialogueList = InDialogueList;
	TutorialSubsystem->SetTutorialActionSequence(InActionSequence);
	TutorialSubsystem->StartTutorial();
}

bool UShopTutorialPresenter::ReportAction(EShopTutorialAction Action)
{
	return IsValid(TutorialSubsystem) && TutorialSubsystem->ReportAction(Action);
}

void UShopTutorialPresenter::SetDimMaskHoleFromWidget(UWidget* TargetWidget)
{
	SetHighlightBoxFromWidget(TargetWidget);
}

void UShopTutorialPresenter::SetHighlightBoxFromWidget(UWidget* TargetWidget, bool bKeepExistingBoxes)
{
	if (!IsValid(TutorialWidget))
	{
		return;
	}
	TutorialWidget->SetHighlightTargetWidget(TargetWidget, bKeepExistingBoxes);
}

void UShopTutorialPresenter::SetHighlightBoxFromActor(AActor* TargetActor, FVector2D MinimumScreenSize, bool bKeepExistingBoxes)
{
	if (IsValid(TutorialWidget))
	{
		TutorialWidget->SetHighlightTargetActor(TargetActor, MinimumScreenSize, bKeepExistingBoxes);
	}
}

/*튜토리얼 내용*/
void UShopTutorialPresenter::ShowTutorialStep(
	int32 StepIndex,
	EShopTutorialAction CurrentAction,
	EShopTutorialAction NextAction)
{
	if (!IsValid(TutorialWidget))
	{
		return;
	}
	
	TutorialWidget->SetHighlightOverlayVisible(false);
	
	/*위젯 클릭 설정*/
	switch (NextAction)
	{
		case EShopTutorialAction::NavigatorBarOpened:

			if (IsValid(NavigationBar))
			{
				NavigationBar->SetNavigationButtonsLocked(false, EShopPage::None);
				PageChangePresenter->SetShopUISelectActorsEnabledForPage(EShopPage::None);
				SetDimMaskHoleFromWidget(NavigationBar->GetNavigationToggleButton());
			}	
			break;
		
		case EShopTutorialAction::PageChangedBoss:
			if (IsValid(NavigationBar))
			{
				NavigationBar->SetNavigationButtonsLocked(false, EShopPage::Boss);
				NavigationBar->SetNavigationToggleLocked(true);
				SetDimMaskHoleFromWidget(NavigationBar->GetBossButton());
			}
			if (IsValid(PageChangePresenter))
			{
				PageChangePresenter->SetShopUISelectActorsEnabledForPage(EShopPage::Boss);
				if (IsValid(ShopUISelectRegistry))
				{
					SetHighlightBoxFromActor(ShopUISelectRegistry->GetBossUISelectActor(), FVector2D(0.2f, 0.3f), true);
				}
			}
			break;

		case EShopTutorialAction::PageChangedWeapon:
			
			TutorialWidget->SetHighlightOverlayVisible(false);
			PageChangePresenter->HandlePageRequested(EShopPage::UnlockWeapon);
			break;

		case EShopTutorialAction::PageChangedCoin:
			TutorialWidget->SetHighlightOverlayVisible(false);
			PageChangePresenter->HandlePageRequested(EShopPage::Coin);
			break;

		case EShopTutorialAction::PageChangedCard:
			TutorialWidget->SetHighlightOverlayVisible(false);
			PageChangePresenter->HandlePageRequested(EShopPage::Card);
			break;

		case EShopTutorialAction::PageChangedItem:
			TutorialWidget->SetHighlightOverlayVisible(false);
			PageChangePresenter->HandlePageRequested(EShopPage::Item);
			break;

		case EShopTutorialAction::PageChangedMain:
			TutorialWidget->SetHighlightOverlayVisible(false);
			PageChangePresenter->HandlePageRequested(EShopPage::Main);
			break;

		
		case EShopTutorialAction::WeaponUnlocked:
			if (IsValid(UnlockWeaponPresenter))
			{
				if (UW_UnlockWeaponWidget* ShopUnlockWidget = UnlockWeaponPresenter->GetShopUnlockWidget())
				{
					if (UW_UnlockWeaponSlotContainer* SlotContainer = ShopUnlockWidget->GetUnlockWeaponSlotContainer(); IsValid(SlotContainer))
					{
						SetDimMaskHoleFromWidget(SlotContainer->GetUnlockWeaponBorder());
					}
				}
			}
			break;

		/*코인관련 튜토리얼*/
		case EShopTutorialAction::CoinSlotPurchased:
			ShopCoinWidget->GetBuyCoinSlotContainer()->SetCoinSlotPurchaseInputEnabled(true);
			ShopCoinWidget->GetShopCoinSlotContainer()->SetCoinSlotInputEnabled(false);
			CoinPresenter->GetShopCoinUIActor()->SetCoinInteractionEnabled(false);
			SetDimMaskHoleFromWidget(ShopCoinWidget->GetBuyCoinSlotContainer());
			break;

		case EShopTutorialAction::FrontCoinWeaponClicked:
			ShopCoinWidget->GetBuyCoinSlotContainer()->SetOnlyCoinSlotPurchaseEnabled(0);
			ShopCoinWidget->GetShopCoinSlotContainer()->SetCoinSlotInputEnabled(false);
			CoinPresenter->GetShopCoinUIActor()->SetCoinInteractionEnabled(false);
			SetDimMaskHoleFromWidget(ShopCoinWidget->GetShopWeaponSlotContainer());
			break;

		case EShopTutorialAction::BackCoinWeaponClicked:
			ShopCoinWidget->GetBuyCoinSlotContainer()->SetOnlyCoinSlotPurchaseEnabled(1);
			ShopCoinWidget->GetShopCoinSlotContainer()->SetCoinSlotInputEnabled(false);
			CoinPresenter->GetShopCoinUIActor()->SetCoinInteractionEnabled(false);
			SetDimMaskHoleFromWidget(ShopCoinWidget->GetShopWeaponSlotContainer());
			break;

		case EShopTutorialAction::CoinSideChanged:
			ShopCoinWidget->GetBuyCoinSlotContainer()->SetOnlyCoinSlotPurchaseEnabled(-1);
			ShopCoinWidget->GetBuyCoinSlotContainer()->SetCoinSlotPurchaseInputEnabled(false);
			ShopCoinWidget->GetShopCoinSlotContainer()->SetCoinSlotInputEnabled(true);
			CoinPresenter->GetShopCoinUIActor()->SetCoinInteractionEnabled(true);
			if (UW_ShopCoinSlotContainer* SlotContainer = ShopCoinWidget->GetShopCoinSlotContainer(); IsValid(SlotContainer))
			{
				const auto& CoinSlots = SlotContainer->GetCoinSlots();
				if (CoinSlots.IsValidIndex(0) && IsValid(CoinSlots[0]))
				{
					SetHighlightBoxFromWidget(CoinSlots[0]->GetBackWeaponImageButton());
				}
			}
			if (IsValid(CoinUIActor))
			{
				SetHighlightBoxFromActor(CoinUIActor, FVector2D(0.3f, 0.3f), true);
			}
			break;

		case EShopTutorialAction::CoinCountIncreased:

			ShopCoinWidget->GetBuyCoinSlotContainer()->SetCoinSlotPurchaseInputEnabled(false);
			ShopCoinWidget->GetShopCoinSlotContainer()->SetCoinSlotInputEnabled(true);
			CoinPresenter->GetShopCoinUIActor()->SetCoinInteractionEnabled(false);
			if (UW_ShopCoinSlotContainer* SlotContainer = ShopCoinWidget->GetShopCoinSlotContainer(); IsValid(SlotContainer))
			{
				const auto& CoinSlots = SlotContainer->GetCoinSlots();
				if (CoinSlots.IsValidIndex(0) && IsValid(CoinSlots[0]))
				{
					SetHighlightBoxFromWidget(CoinSlots[0]->GetIncreaseButton());
				}
			}
			break;

		/*카드관련 튜토리얼*/

		case EShopTutorialAction::CardSelected:
			ShopCardMainWidget->GetShopCardSlotContainer()->SetCardSlotInputEnabled(false);
			if (UW_ShopPlayerCardSlotContainer* Container = ShopCardMainWidget->GetShopPlayerCardSlotContainer(); IsValid(Container))
			{
				SetDimMaskHoleFromWidget(Container->GetShopPlayerCardBorder());
			}
			break;
		case EShopTutorialAction::CardPurchased:
			if (UW_ShopCardSlotContainer* Container = ShopCardMainWidget->GetShopCardSlotContainer(); IsValid(Container))
			{
				SetDimMaskHoleFromWidget(Container->GetShopCardBorder());
			}
			break;
		/*아이템 */
		case EShopTutorialAction::ItemPurchased:

			ShopItemWidget->GetShopItemPurchasePopup()->SetOnlyPurchaseControlsEnabled(true);
			if (UW_ShopItemSlotContainer* Container = ShopItemWidget->GetShopItemSlotContainer(); IsValid(Container))
			{
				SetDimMaskHoleFromWidget(Container->GetShopItemBorder());
			}
			break;

		case EShopTutorialAction::ItemSold:
			ShopItemWidget->GetShopItemSlotContainer()->SetItemSlotInputEnabled(false);
			ShopItemWidget->GetShopItemSellPopup()->SetOnlySellControlsEnabled(true);
			if (UW_ShopPlayerItemSlotContainer* Container = ShopItemWidget->GetShopPlayerItemSlotContainer(); IsValid(Container))
			{
				SetDimMaskHoleFromWidget(Container->GetShopPlayerItemBorder());
			}
			break;
			
		case EShopTutorialAction::EndTutorial:
			if (UWorld* World = GetWorld(); IsValid(World))
				if (AGameMode_Shop* GameMode = World->GetAuthGameMode<AGameMode_Shop>(); IsValid(GameMode))
				{
					TutorialWidget->SetTutorialActive(false);
					GameMode->ChangeBattleLevel();
				}
			return;

		default:
			break;
	}


	/*대화창 설정*/
	
	
	if (!TutorialDialogueList.IsValidIndex(StepIndex))
	{
		TutorialWidget->SetTutorialActive(false);
		return;
	}

	if (IsValid(TutorialFlow) &&
		TutorialFlow->GetDialogueBoxPositions().IsValidIndex(StepIndex) &&
		TutorialFlow->GetDialogueBoxSizes().IsValidIndex(StepIndex))
	{
		TutorialWidget->SetDialogueBoxLayout(
			TutorialFlow->GetDialogueBoxPositions()[StepIndex],
			TutorialFlow->GetDialogueBoxSizes()[StepIndex]);
	}

	const bool bCanClickDialogue = NextAction == EShopTutorialAction::DialogueClicked;
	TutorialWidget->SetTutorialInputEnabled(bCanClickDialogue);
	TutorialWidget->SetDialogueText(TutorialDialogueList[StepIndex]);
}


void UShopTutorialPresenter::HandleTutorialStepChanged(
	int32 NewStepIndex,
	EShopTutorialAction CompletedAction,
	EShopTutorialAction NextAction)
{
	ShowTutorialStep(NewStepIndex, CompletedAction, NextAction);
	OnTutorialStepChanged.Broadcast(NewStepIndex, CompletedAction, NextAction);
}

void UShopTutorialPresenter::HandleTutorialStarted(int32 InitialStepIndex)
{
	const EShopTutorialAction NextAction = IsValid(TutorialSubsystem)
		? TutorialSubsystem->GetExpectedAction()
		: EShopTutorialAction::None;
	ShowTutorialStep(InitialStepIndex, EShopTutorialAction::None, NextAction);
	OnTutorialStarted.Broadcast(InitialStepIndex);
}

void UShopTutorialPresenter::HandleTutorialCompleted()
{
	if (IsValid(TutorialWidget))
	{
		TutorialWidget->SetTutorialActive(false);
	}

	OnTutorialCompleted.Broadcast();
}



void UShopTutorialPresenter::HandleDialogueClicked()
{
	ReportAction(EShopTutorialAction::DialogueClicked);
}

void UShopTutorialPresenter::HandleNavigationToggleClicked()
{
	ReportAction(EShopTutorialAction::NavigatorBarOpened);
}

void UShopTutorialPresenter::HandleCardPurchased(int32 CardID)
{
	
	ReportAction(EShopTutorialAction::CardPurchased);
}

void UShopTutorialPresenter::HandlePlayerCardSelected(int32 CardID)
{
	ReportAction(EShopTutorialAction::CardSelected);
}

void UShopTutorialPresenter::HandleItemPurchaseClicked(int32 ItemID)
{
	ReportAction(EShopTutorialAction::ItemPurchaseClicked);
}

void UShopTutorialPresenter::HandleItemPurchased(int32 ItemID, int32 Count)
{
	ReportAction(EShopTutorialAction::ItemPurchased);
}

void UShopTutorialPresenter::HandleItemSold(int32 ItemID, int32 Count)
{
	ReportAction(EShopTutorialAction::ItemSold);
}

void UShopTutorialPresenter::HandleWeaponUnlocked(int32 WeaponID)
{
	ReportAction(EShopTutorialAction::WeaponUnlocked);
}

void UShopTutorialPresenter::HandleCoinSlotPurchased(int32 Level)
{
	ReportAction(EShopTutorialAction::CoinSlotPurchased);
}

void UShopTutorialPresenter::HandleCoinWeaponClicked(int32 WeaponID)
{
	if(WeaponID == 1)
		ReportAction(EShopTutorialAction::FrontCoinWeaponClicked);
	else
		ReportAction(EShopTutorialAction::BackCoinWeaponClicked);
}

void UShopTutorialPresenter::HandleCoinSideChanged(bool bIsFrontSide)
{
	
	CoinPresenter->GetShopCoinUIActor()->SetCoinInteractionEnabled(false);
	ReportAction(EShopTutorialAction::CoinSideChanged);
}

void UShopTutorialPresenter::HandleCoinCountIncreased(int32 SlotIndex, int32 Count)
{
	ReportAction(EShopTutorialAction::CoinCountIncreased);
}

void UShopTutorialPresenter::HandlePageChangeStart(EShopPage TargetPage)
{
	if (IsValid(TutorialWidget))
	{
		TutorialWidget->SetHighlightOverlayVisible(false);
		TutorialWidget->SetDialogueBorderVisible(false);
	}
}

void UShopTutorialPresenter::HandlePageMoveCompleted(EShopPage CompletedPage)
{
	
	TutorialWidget->SetHighlightOverlayVisible(false);
	switch (CompletedPage)
	{
		case EShopPage::Boss:
			ReportAction(EShopTutorialAction::PageChangedBoss);
			NavigationBar->SetNavigationButtonsLocked(true, EShopPage::None);
			break;
		
		case EShopPage::UnlockWeapon:
			ReportAction(EShopTutorialAction::PageChangedWeapon);
			break;

		case EShopPage::Coin:
			ReportAction(EShopTutorialAction::PageChangedCoin);
			break;

		case EShopPage::Card:
			ReportAction(EShopTutorialAction::PageChangedCard);
			break;

		case EShopPage::Item:
			ReportAction(EShopTutorialAction::PageChangedItem);
			break;
		case EShopPage::Main:
			ReportAction(EShopTutorialAction::PageChangedMain);
			break;
		default:
			ReportAction(EShopTutorialAction::None);
			break;

	}
}
