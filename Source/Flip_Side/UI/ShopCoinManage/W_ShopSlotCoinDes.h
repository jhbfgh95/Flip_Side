// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDescriptionDataTypes.h"
#include "Styling/SlateTypes.h"
#include "W_ShopSlotCoinDes.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopSlotCoinDescriptionHoverChanged, bool);
DECLARE_MULTICAST_DELEGATE(FOnShopSlotCoinDescriptionHideFinished);

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopSlotCoinDes : public UUserWidget
{
	GENERATED_BODY()

	protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& Geometry, const FPointerEvent& Event) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& Event) override;

	private:
	UPROPERTY(meta= (BindWidget))
	class UTextBlock* WeaponNameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> AbilityButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RangeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> ContentSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCoinDescriptionSectionWidget> DescriptionSection;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UPanelWidget> BookmarkBox;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Description")
	TSubclassOf<class UCoinDescriptionBookmarkWidget> BookmarkWidgetClass;
	UPROPERTY(Transient)
	TArray<FCoinDescriptionSectionData> Sections;
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UCoinDescriptionBookmarkWidget>> Bookmarks;
	int32 SelectedSectionIndex = INDEX_NONE;
	bool bDetailed = false;
	bool bDescriptionHovered = false;
	FButtonStyle AbilityButtonDefaultStyle;
	FButtonStyle RangeButtonDefaultStyle;
	bool bHasButtonDefaultStyles = false;
	FTimerHandle HideTimer;
	void ClearBookmarks();
	void RebuildBookmarks();
	void SelectDescription(int32 Index);
	void HandleBookmarkClicked(bool bFrontFace, int32 Index);
	void HideIfNotHovered();
	void SetActivePage(int32 PageIndex);
	void RefreshPageButtonStyles(int32 ActivePageIndex);
	void SetPageButtonSelected(class UButton* Button, const FButtonStyle& DefaultStyle, bool bSelected);
	UFUNCTION()
	void HandleAbilityButtonClicked();
	UFUNCTION()
	void HandleRangeButtonClicked();

	public:
	void SetWeaponDescription(const struct FFaceData& Weapon, const TArray<struct FKeywordDefinitionData>& Keywords);
	UFUNCTION(BlueprintCallable, Category = "Weapon Description")
	void SetDetailed(bool bInDetailed);
	void RequestHide();
	void SetExplainTextEmpty();

	// Presenter는 이 이벤트를 받아 설명 복원 여부를 결정합니다.
	FOnShopSlotCoinDescriptionHoverChanged OnDescriptionHoverChanged;
	FOnShopSlotCoinDescriptionHideFinished OnHideFinished;
	
};
