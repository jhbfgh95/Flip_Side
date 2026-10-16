// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "DataTypes/KeywordDataTypes.h"
#include "Styling/SlateTypes.h"
#include "W_CardWidget.generated.h"

/**
 * 
 */


UCLASS()
class FLIP_SIDE_API UW_CardWidget : public UUserWidget
{
	GENERATED_BODY()
	
	protected:
	virtual void NativeConstruct() override;

	protected:
	UPROPERTY(meta = (BindWidget))
	class UBorder* CardImage;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* CardIconImage;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CardTitle;
	
	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* CardDescription;

	// 배틀 카드 설명 WBP 내부에 배치하는 추가 정보 전용 레이어입니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> AdditonalInfoLayer;
	UPROPERTY(EditDefaultsOnly, Category = "Card Description|Additional Info")
	TSubclassOf<class UCardGoldAppliedInfoWidget> GoldAppliedInfoWidgetClass;
	UPROPERTY(Transient)
	TObjectPtr<class UCardGoldAppliedInfoWidget> GoldAppliedInfoWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Card Description", meta = (ClampMin = "1.0"))
	FVector2D KeywordIconSize = FVector2D(24.0f, 24.0f);
	UPROPERTY(EditDefaultsOnly, Category = "Card Description", meta = (ClampMin = "0.0"))
	float KeywordIconNameSpacing = 4.0f;
	UPROPERTY(Transient)
	TMap<FName, FKeywordDefinitionData> KeywordDefinitions;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> CardIconMaterialInstance;

	public:
	//void SetCardIcon(UImage* Image);
	//void InitCard(UTexture2D* IconImage, FString Title,FString Description);
	void InitCard(FCardData CardData);
	void InitBattleCard(const FBattleCardSlotViewData& InData);
	void HideAdditionalInfo();
	TSharedPtr<SWidget> CreateKeywordDisplay(FName Code, const FTextBlockStyle& Style);
};
