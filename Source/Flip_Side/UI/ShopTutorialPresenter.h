#pragma once

#include "CoreMinimal.h"
#include "DataTypes/ShopPageTypes.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "UI/ShopCard/W_ShopCardMainWidget.h"
#include "UI/ShopCard/W_ShopCardSlotContainer.h"
#include "UI/ShopCoinManage/W_BuyCoinSlotContainer.h"
#include "UI/ShopCoinManage/W_ShopCoinWidget.h"
#include "UI/ShopItem/W_ShopItemPurchasePopup.h"
#include "UI/ShopItem/W_ShopItemSellPopup.h"
#include "UI/ShopItem/W_ShopItemWidget.h"
#include "UObject/Object.h"
#include "ShopTutorialPresenter.generated.h"

class UShopTutorialWSubsystem;
class UShopCardPresenter;
class UShopItemPresenter;
class UShopCoinPresenter;
class UUnlockWeaponPresenter;
class UShopPageChangePresenter;
class UW_ShopTutorialWidget;
class UShopTutorialFlow;
class UW_ShopWidgetContainer;
class UW_ShopNavigationBar;
class UWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnShopTutorialPresenterStepChanged,
	int32,
	NewStepIndex,
	EShopTutorialAction,
	CompletedAction,
	EShopTutorialAction,
	NextAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopTutorialPresenterStarted, int32, InitialStepIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopTutorialPresenterCompleted);

UCLASS()
class FLIP_SIDE_API UShopTutorialPresenter : public UObject
{
	GENERATED_BODY()

public:
	void InitPresenter(
		UShopTutorialWSubsystem* InTutorialSubsystem,
		UW_ShopTutorialWidget* InTutorialWidget,
		UW_ShopWidgetContainer* InShopWidgetContainer);

	void SetShopPresenters(
		UShopCardPresenter* InCardPresenter,
		UShopItemPresenter* InItemPresenter,
		UShopCoinPresenter* InCoinPresenter,
		UUnlockWeaponPresenter* InUnlockWeaponPresenter,
		UShopPageChangePresenter* InPageChangePresenter);

	void StartTutorial();
	void StartTutorialWithSequence(
		const TArray<FText>& InDialogueList,
		const TArray<EShopTutorialAction>& InActionSequence);

	bool ReportAction(EShopTutorialAction Action);

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialPresenterStepChanged OnTutorialStepChanged;

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialPresenterStarted OnTutorialStarted;

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialPresenterCompleted OnTutorialCompleted;

private:
	UPROPERTY()
	TObjectPtr<UShopTutorialWSubsystem> TutorialSubsystem;

	UPROPERTY()
	TObjectPtr<UShopCardPresenter> CardPresenter;

	UPROPERTY()
	TObjectPtr<UShopItemPresenter> ItemPresenter;

	UPROPERTY()
	TObjectPtr<UShopCoinPresenter> CoinPresenter;

	UPROPERTY()
	TObjectPtr<UUnlockWeaponPresenter> UnlockWeaponPresenter;

	UPROPERTY()
	TObjectPtr<UShopPageChangePresenter> PageChangePresenter;

	UPROPERTY()
	TObjectPtr<UW_ShopCoinWidget> ShopCoinWidget;

	UPROPERTY()
	TObjectPtr<UW_ShopCardMainWidget> ShopCardMainWidget;

	UPROPERTY()
	TObjectPtr<UW_ShopItemWidget> ShopItemWidget;

	UPROPERTY()
	TObjectPtr<UW_ShopTutorialWidget> TutorialWidget;

	UPROPERTY()
	TObjectPtr<UW_ShopWidgetContainer> ShopWidgetContainer;

	UPROPERTY()
	TObjectPtr<UW_ShopNavigationBar> NavigationBar;

	UPROPERTY()
	TObjectPtr<UShopTutorialFlow> TutorialFlow;

	TArray<FText> TutorialDialogueList;

	void ShowTutorialStep(
		int32 StepIndex,
		EShopTutorialAction CurrentAction,
		EShopTutorialAction NextAction);

	void SetDimMaskHoleFromWidget(UWidget* TargetWidget);

	UFUNCTION()
	void HandleDialogueClicked();

	UFUNCTION()
	void HandleNavigationToggleClicked();

	UFUNCTION()
	void HandleCardPurchased(int32 CardID);

	UFUNCTION()
	void HandlePlayerCardSelected(int32 CardID);

	UFUNCTION()
	void HandleItemPurchaseClicked(int32 ItemID);

	UFUNCTION()
	void HandleItemPurchased(int32 ItemID, int32 Count);

	UFUNCTION()
	void HandleItemSold(int32 ItemID, int32 Count);

	UFUNCTION()
	void HandleWeaponUnlocked(int32 WeaponID);

	UFUNCTION()
	void HandleCoinSlotPurchased(int32 Level);

	UFUNCTION()
	void HandleCoinWeaponClicked(int32 WeaponID);

	UFUNCTION()
	void HandleCoinSideChanged(bool bIsFrontSide);

	UFUNCTION()
	void HandleCoinCountIncreased(int32 SlotIndex, int32 Count);

	UFUNCTION()
	void HandlePageMoveCompleted(EShopPage CompletedPage);

	UFUNCTION()
	void HandleTutorialStepChanged(
		int32 NewStepIndex,
		EShopTutorialAction CompletedAction,
		EShopTutorialAction NextAction);

	UFUNCTION()
	void HandleTutorialStarted(int32 InitialStepIndex);

	UFUNCTION()
	void HandleTutorialCompleted();
};
