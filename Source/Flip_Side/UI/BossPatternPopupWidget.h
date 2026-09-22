// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossDataTypes.h"
#include "BossPatternPopupWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UBossPatternPopupWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetPatternInfo(const FBossHUDData& InData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PatternNameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URichTextBlock> PatternDescriptionText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AttackPointText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ShieldHealText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ConditionalAttackPointText;
	// 제목/아이콘도 함께 숨기려면 각 스탯 행을 이 이름의 컨테이너로 묶습니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> AttackPointContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> ShieldHealContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> ConditionalAttackPointContainer;
};
