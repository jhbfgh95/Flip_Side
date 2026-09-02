// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopCardBase.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredShopCardSlot, int32, CardID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoveredShopCardSlot);

UCLASS()
class FLIP_SIDE_API UW_ShopCardBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnHoveredShopCardSlot OnHoveredShopCardSlot;
	FOnUnhoveredShopCardSlot OnUnhoveredShopCardSlot;
};
