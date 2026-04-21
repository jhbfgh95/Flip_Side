// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_SellItemButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_SellItemButton : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	class UShopItemWSubsystem* ItemSubsystem;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ItemButton;
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

private:
	FItemData ItemData;

private:
	UFUNCTION()
	void HoverItem();

	UFUNCTION()
	void UnHoverItem();

	UFUNCTION()
	void BuyItem();
public:
	void InitButton(FItemData InitItemData);
};
