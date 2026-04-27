// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossDataTypes.h"
#include "BossPatternBase.h"
#include "W_ShopBossInfo.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopBossInfo : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	class UPanelWidget* BossInfoPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	class UPanelWidget* PatternInfoPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	class UHorizontalBox* PatternButtonBox;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* BossImage;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* BossNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* BossAbilityText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* PatternTitleText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* PatternRangeImage;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* PatternIconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* PatternNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* PatternAttackText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* PatternDescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_1;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_2;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_3;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_4;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_5;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_6;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_7;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_8;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_9;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PatternButton_10;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	FBossData CurrentBossData;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	int32 CurrentPatternIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	TArray<FPatternData> CurrentPatternDataList;

public:
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void RefreshPreparedBossInfo();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void SetBossInfo(const FBossData& BossData);

	void SetBossInfo(const FBossData& BossData, const TArray<FPatternData>& PatternDataList);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void SelectPattern(int32 PatternIndex);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void SetPatternButtonCount(int32 PatternCount);

protected:
	UFUNCTION()
	void SelectPattern1();

	UFUNCTION()
	void SelectPattern2();

	UFUNCTION()
	void SelectPattern3();

	UFUNCTION()
	void SelectPattern4();

	UFUNCTION()
	void SelectPattern5();

	UFUNCTION()
	void SelectPattern6();

	UFUNCTION()
	void SelectPattern7();

	UFUNCTION()
	void SelectPattern8();

	UFUNCTION()
	void SelectPattern9();

	UFUNCTION()
	void SelectPattern10();

	void CachePatternButtons();
	void RefreshBossTexts();
	void RefreshPatternTexts();
	void RefreshPatternButtonState();
	void ClearBossInfo();
	void ClearPatternInfo();

	UPROPERTY()
	TArray<TObjectPtr<class UButton>> PatternButtons;
};
