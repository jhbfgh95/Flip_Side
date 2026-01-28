// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/ItemDataTypes.h"
#include "ShopItemWSubsystem.generated.h"

/**
 * 
 */
//코인이 클래스가 선택 됬을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemSelecte, FItemData, ItemInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemHovere, int32, ItemInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemUnHovere);

UCLASS()
class FLIP_SIDE_API UShopItemWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
protected:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	FItemSelecte OnItemSelected;

	FItemHovere OnItemHovered;
	
	FItemUnHovere OnItemUnHovered;


private:
	//상점 아이템 구매 목록
	TArray<FItemData> ShopItemArray;

	//플레이어 아이템 목록
	TArray<FSelectItem> PlayerItemArray;

	//아이템 선택 함수
	FItemData GetSelectItem();
	//아이템 정보 반환 함수

public:
	void HoverShopItem(int32 ItemNum);
	void UnHoverShopItem();
	void HoverPlayerItem(int32 ItemNum);
	void UnHoverPlayerItem(int32 ItemNum);
	void BuyItem(int32 Num);
	void SellItem(int32 Num);

};
