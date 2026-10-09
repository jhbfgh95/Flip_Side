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
class AActor;
class AShopUISelectRegistry;
class AShopCoinUIActor;

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
	void InitShopActors(AShopUISelectRegistry* InShopUISelectRegistry, AShopCoinUIActor* InCoinUIActor);
	void StartTutorialWithSequence(
		const TArray<FText>& InDialogueList,
		const TArray<EShopTutorialAction>& InActionSequence);

	bool ReportAction(EShopTutorialAction Action);

	void SetHighlightBoxFromWidget(UWidget* TargetWidget, bool bKeepExistingBoxes = false);
	// 메시 바운드와 액터 중심의 최소 영역을 화면에 투영합니다. 크기는 화면 비율입니다.
	void SetHighlightBoxFromActor(AActor* TargetActor, FVector2D MinimumScreenSize = FVector2D(1.f, 1.f), bool bKeepExistingBoxes = false);

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialPresenterStepChanged OnTutorialStepChanged;

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialPresenterStarted OnTutorialStarted;

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialPresenterCompleted OnTutorialCompleted;

private:
	UPROPERTY()
	TObjectPtr<AShopUISelectRegistry> ShopUISelectRegistry;

	UPROPERTY()
	TObjectPtr<AShopCoinUIActor> CoinUIActor;

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
	void HandlePageChangeStart(EShopPage TargetPage);

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
