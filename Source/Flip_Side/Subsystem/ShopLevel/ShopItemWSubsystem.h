// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/ItemDataTypes.h"
#include "ShopItemWSubsystem.generated.h"
#define MAX_ITEM_COUNT 10
/**
 * 
 */
//코인이 클래스가 선택 됬을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemSelect, FItemData, ItemInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemHover, FItemData, ItemInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemBuy, int32, InvenIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemUnHovere);

UCLASS()
class FLIP_SIDE_API UShopItemWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
protected:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


private:
	class UDataManagerSubsystem* DM;

public:
	//아이템 선택 됬을때 델리게이트
	FItemSelect OnItemSelected;
	//아이템을 올려둘 시
	FItemHover OnItemHovered;
	//아이템에서 마우스를 땔때
	FItemUnHovere OnItemUnHovered;

	FItemBuy OnItemBuy;

private:

	//상점 아이템 구매 목록
	TArray<FItemData> ShopItemArray;
	//플레이어 아이템 목록
	TArray<FSelectItem> PlayerItemArray;
	

private:

	//기본 아이템 데이터
	FSelectItem DefaultSelecttemData;
	FItemData DefaultItemData;

private:

	int32 GetEmptyInvenIndex(int32 ItemID);
	//해당 아이디를 가진 아이템이 플레이어의 인벤토리에 있는지
	int32 GetSameItemInvenIndex(int32 ItemID);
public:
	//상점 아이템에 마우스를 올려놧을 때
	void HoverItem(FItemData ItemNum);
	//상점 아이템에서 마우스를 땠을떄
	void UnHoverItem();
	
	FItemData GetItemDataByShopIndex(int32 ShopIndex);
	
	FSelectItem GetItemDataByItemID(int32 ItemID);

	void BuyItem(FItemData ItemData);
	
	int32 GetPlayerItemNum();

	FSelectItem GetPlayerItem(int32 index);

	TArray<FItemData> GetShopItemList();

};
