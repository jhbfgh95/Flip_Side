// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossDataTypes.h"
#include "BattleBossPatternHUDWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBattleBossPatternWidgetHovered);
DECLARE_MULTICAST_DELEGATE(FOnBattleBossPatternWidgetUnhovered);

/**
 * 전투 HUD의 현재 보스 패턴 표시 위젯입니다.
 */
UCLASS()
class FLIP_SIDE_API UBattleBossPatternHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetBossHUDData(const FBossHUDData& InData);

	FOnBattleBossPatternWidgetHovered OnPatternHovered;
	FOnBattleBossPatternWidgetUnhovered OnPatternUnhovered;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> PatternButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> PatternImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> PatternName;

private:
	UFUNCTION()
	void HandlePatternHovered();

	UFUNCTION()
	void HandlePatternUnhovered();

	void ClearPatternData();
};
