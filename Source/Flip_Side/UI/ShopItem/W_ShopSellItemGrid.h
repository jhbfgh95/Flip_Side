// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopSellItemGrid.generated.h"

/**
 * 
 */
class UVerticalBox;
UCLASS()
class FLIP_SIDE_API UW_ShopSellItemGrid : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
private:
	class UShopItemWSubsystem* ShopItemSubsystem;

protected:
	TArray<class UW_SellItemButton*> SellItems;
	/*
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* SellItemGrid;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemBuyDescrip;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> SellItemButton;*/

	TArray<class UW_ShopItemSlot*> SellItemSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> SellItemSlot;
private:
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 ColumnCount = 3;


	UFUNCTION()
	void ShowDescrip(FItemData ItemInfo);
	
	UFUNCTION()
	void HideDescrip();

};
