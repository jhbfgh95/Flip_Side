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

protected:
	// 상점·보유·선택 카드 칸 모두 여기서 마우스 올리기 소리를 냅니다(각 칸은 Super를 먼저 부름).
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
