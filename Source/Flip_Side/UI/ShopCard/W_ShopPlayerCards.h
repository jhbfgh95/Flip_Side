// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopPlayerCards.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerCards : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	class UShopCardWSubsystem* ShopCardSubsystem;

private:

	TArray<class UW_ShopPlayerCard*> PlayerCardWidget;
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* MainCanvas;
private:
	UFUNCTION()
	void SetPlayerCard();

};
