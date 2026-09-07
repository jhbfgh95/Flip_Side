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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemBuy, int32, InvenIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShopItemWarning, int32, WarningCode);

UCLASS()
class FLIP_SIDE_API UShopItemWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
protected:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	class UDataManagerSubsystem* DM;
	class UMoneyGISubsystem* MoneySubsystem;

public:
	FItemBuy OnItemBuy;
	FShopItemWarning OnShopItemWarning;
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
	//해당 아이디를 가진 아이템이 플레이어의 인벤토리에 있는지
	int32 GetAddItemInvenIndex(int32 ItemID);
public:

	bool CanBuyItem(int32 Price, int32 ItemCount);

	FItemData GetItemDataByShopIndex(int32 ShopIndex);
	
	FSelectItem GetItemDataByItemID(int32 ItemID);

	int32 GetSameItemCountByItemID(int32 ItemID);

	int32 GetSameItemCountByIndex(int32 InvenIndex);

	bool BuyItem(FItemData ItemData, int32 ItemCount);

	bool SellItem(FItemData ItemData, int32 ItemCount);

	int32 GetPlayerItemNum();

	FSelectItem GetPlayerItem(int32 index);
	
	FItemData GetPlayerItemData(int32 index);

	TArray<FItemData> GetShopItemList();

	TArray<FSelectItem> GetPlayerItemArray();

public:
	void ShopItemWarning(int32 WarningCode);
	
};
