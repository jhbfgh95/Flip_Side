// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDescriptionDataTypes.h"
#include "W_WeaponDescription.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_WeaponDescription : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 같은 이름의 BP Image를 DB 아이콘에 직접 연결합니다. Brush 크기와 색상은 유지합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> AttackPowerIcon;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> WeaponPowerIcon;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> CountIcon;
	UPROPERTY(EditDefaultsOnly, Category = "UI|Stat Icons")
	FName StatIconTextureParameter = TEXT("Icon");

	protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	private:
	UPROPERTY(meta= (BindWidget))
	class UTextBlock* WeaponNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AttackPower;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeaponPower;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountText;

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

	void ClearBookmarks();

	void RebuildBookmarks();

	void SelectDescription(int32 Index);

	void HandleBookmarkClicked(bool bFrontFace, int32 Index);

	public:

	void SetWeaponDescription(const struct FFaceData& Weapon, const TArray<struct FKeywordDefinitionData>& Keywords);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon Description")
	void SetDetailed(bool bInDetailed);

	void SetExplainTextEmpty();

};
