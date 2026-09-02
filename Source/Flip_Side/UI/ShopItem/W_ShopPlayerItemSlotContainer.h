// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopPlayerItemSlotContainer.generated.h"

/**
 * 
 */
class UVerticalBox;
class UW_ShopPlayerItemSlot;
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerItemSlotContainer : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY()
	TArray<TObjectPtr<UW_ShopPlayerItemSlot>> PlayerItemSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> PlayerItemSlot;

public:
	void InitWidget(TArray<FItemData> InItemDataArray, TArray<FSelectItem> InSelectItemData);
	
	TArray<TObjectPtr<UW_ShopPlayerItemSlot>> GetPlayerItemSlots();

};
