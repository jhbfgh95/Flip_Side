// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/W_CardWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopPlayerCard.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerCard : public UW_CardWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	class UUnlockGISubsystem* UnlockSubsystem;
	class UShopCardWSubsystem* ShopCardSubsystem;

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(meta = (BindWidget), Transient)
	class UCanvasPanel* MainCanvas;

	FCardData CardData;

private:
	bool IsInitState = false;


private:
	UPROPERTY(meta= (BindWidgetAnim), Transient)
	class UWidgetAnimation* SelectAnim;
	UPROPERTY(meta= (BindWidgetAnim), Transient)
	class UWidgetAnimation* UnSelectAnim;
	UPROPERTY(meta= (BindWidgetAnim), Transient)
	class UWidgetAnimation* ChangeCardAnim;

private:

	void CheckSelectCard();	


public:

	void InitShopCard(FCardData CardDataValue, bool IsUnlock);
};
