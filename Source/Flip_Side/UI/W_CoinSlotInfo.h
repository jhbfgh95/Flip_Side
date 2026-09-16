// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoinDataTypes.h"
#include "UI/CoinDescriptionSectionWidget.h"
#include "W_CoinSlotInfo.generated.h"

UCLASS()
class FLIP_SIDE_API UW_CoinSlotInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UContentWidget> FrontDescriptionContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UContentWidget> BackDescriptionContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> FrontBookmarkContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> BackBookmarkContainer;
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description")
	TSubclassOf<UCoinDescriptionSectionWidget> DescriptionSectionWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description")
	TSubclassOf<class UCoinDescriptionBookmarkWidget> DescriptionBookmarkWidgetClass;
	// 기본 경로는 Controller의 DB 데이터입니다. 수동 BP 데이터는 명시적인 미리보기에서만 사용합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description|Preview")
	bool bUsePreviewDescriptions = false;
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description")
	TMap<int32, FCoinWeaponDescriptionData> WeaponDescriptions;
	UFUNCTION(BlueprintImplementableEvent, Category = "Coin Description")
	void OnDescriptionWeaponsChanged(int32 FrontWeaponID, int32 BackWeaponID);

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* HoveredFrontWeaponIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* HoveredBackWeaponIcon;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HoveredFrontWeaponName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HoveredBackWeaponName;

	/** 앞면 기본 스탯입니다. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FrontAttackPowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FrontWeaponPowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FrontCountText;

	/** 뒷면 기본 스탯입니다. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BackAttackPowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BackWeaponPowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BackCountText;
	
	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinMaxHPText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinSlotNumberText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* CoinCountText;

	UPROPERTY()
	class UMaterialInstanceDynamic* FrontDynamicMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* BackDynamicMaterial;

public:
	void SetCoinSlotInfo(const FBattleCoinSlotViewData& InData);
	UFUNCTION(BlueprintCallable, Category = "Coin Description")
	void SetFaceDescriptions(bool bFrontFace, const TArray<FCoinDescriptionSectionData>& Sections);
	void ResetDescriptionSelection();
	void SetDetailedDescriptions(bool bDetailed);

private:
	void HandleBookmarkClicked(bool bFrontFace, int32 SectionIndex);
	void SelectDescription(bool bFrontFace, int32 SectionIndex);
	bool bShowingDetailedDescriptions = false;
	// 면별 설명 위젯은 한 개만 유지하며 책갈피 클릭 시 내용만 교체합니다.
	UPROPERTY(Transient)
	TObjectPtr<UCoinDescriptionSectionWidget> FrontDescriptionWidget;
	UPROPERTY(Transient)
	TObjectPtr<UCoinDescriptionSectionWidget> BackDescriptionWidget;
	UPROPERTY(Transient)
	TArray<FCoinDescriptionSectionData> FrontDescriptionData;
	UPROPERTY(Transient)
	TArray<FCoinDescriptionSectionData> BackDescriptionData;
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UCoinDescriptionBookmarkWidget>> FrontBookmarks;
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UCoinDescriptionBookmarkWidget>> BackBookmarks;
	int32 CurrentSlotNumber = INDEX_NONE;
	int32 CurrentFrontWeaponID = INDEX_NONE;
	int32 CurrentBackWeaponID = INDEX_NONE;
	int32 SelectedFrontSection = INDEX_NONE;
	int32 SelectedBackSection = INDEX_NONE;
	void SetWeaponInfo(
		bool bFrontFace,
		class UTexture2D* Icon,
		const FText& WeaponName,
		const FWeaponStatDisplayData& StatData,
		const FLinearColor& WeaponColor
	);
};
