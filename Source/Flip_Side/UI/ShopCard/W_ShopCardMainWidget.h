// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCardMainWidget.generated.h"

/**
 * 
 */
class UW_ShopCardSlotContainer;
class UW_ShopCardWidget;

class UW_ShopPlayerCardSlotContainer;
UCLASS()
class FLIP_SIDE_API UW_ShopCardMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCardSlotContainer> ShopCardSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopPlayerCardSlotContainer> ShopPlayerCardSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UW_ShopCardWidget> ShopCardWidget;


public:
	UW_ShopCardSlotContainer* GetShopCardSlotContainer() const;

	UW_ShopPlayerCardSlotContainer* GetShopPlayerCardSlotContainer() const;

	UW_ShopCardWidget* GetShopCardWidget() const;
};
