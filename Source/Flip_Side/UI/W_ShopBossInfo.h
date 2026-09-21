// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossDataTypes.h"
#include "BossPatternBase.h"
#include "BossPatternPreviewActor.h"
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
	class UTextBlock* BossHPText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* BossShieldText;

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
	class UImage* AttackIconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* PatternAttackText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* PatternDescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* PassiveButton;

	UPROPERTY(EditAnywhere, Category = "Boss|Pattern")
	TSubclassOf<class UW_ShopBossPatternButton> PatternButtonWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	FBossDisplayData CurrentBossData;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	int32 CurrentPatternIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	float CurrentStatMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	float CurrentGimmickMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	TArray<FBossPatternDisplayData> CurrentPatternDataList;

public:
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void RefreshPreparedBossInfo();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void SetBossInfo(const FBossDisplayData& BossData);

	void SetBossInfo(const FBossDisplayData& BossData, const TArray<FBossPatternDisplayData>& PatternDataList);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void SelectPattern(int32 PatternIndex);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void SetPatternButtonCount(int32 PatternCount);

protected:
	UFUNCTION()
	void SelectPassive();

	void RebuildPatternButtons(int32 PatternCount);
	void RefreshBossTexts();
	void RefreshPatternTexts();
	void RefreshPatternButtonState();
	void ClearBossInfo();
	void ClearPatternInfo();

	UPROPERTY()
	TArray<TObjectPtr<class UW_ShopBossPatternButton>> PatternButtons;

public:
	// 레벨에 배치된 BossPatternPreviewActor를 블루프린트에서 연결
	UPROPERTY(BlueprintReadWrite, Category = "Preview")
	TObjectPtr<ABossPatternPreviewActor> PatternPreviewActor;
};
