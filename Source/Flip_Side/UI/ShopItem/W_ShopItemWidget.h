// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopItemWidget.generated.h"

/**
 * 
 */
class UW_ShopPlayerItemSlotContainer;
class UW_ShopItemSlotContainer;
UCLASS()
class FLIP_SIDE_API UW_ShopItemWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopPlayerItemSlotContainer> ShopPlayerItemSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopItemSlotContainer> ShopItemSlotContainer;
	
public:
	UFUNCTION()
	UW_ShopPlayerItemSlotContainer* GetShopPlayerItemSlotContainer() const;

	UFUNCTION()
	UW_ShopItemSlotContainer* GetShopItemSlotContainer() const;


};
