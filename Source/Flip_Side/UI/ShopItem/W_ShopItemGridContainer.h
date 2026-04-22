// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopItemGridContainer.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopItemGridContainer : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

private:
	class UShopItemWSubsystem* ShopItemSubsystem;
private:
	UPROPERTY(meta = (BindWidget))
	class UUserWidget* ItemInvenGrid;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemInvenButton;
	
	UPROPERTY(meta = (BindWidget))
	class UUserWidget* ShopGrid;

	UPROPERTY(meta = (BindWidget))
	class UButton* ShopButton;
	
	UFUNCTION()
	void SetShopGrid();
	
	UFUNCTION()
	void SetInvenGrid();
};
