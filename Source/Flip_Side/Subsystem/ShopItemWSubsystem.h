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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemSelecte, FItemData, ItemInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemHovere, int32, ItemInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemUnHovere);

UCLASS()
class FLIP_SIDE_API UShopItemWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
protected:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	FItemSelecte OnItemSelected;

	FItemHovere OnItemHovered;
	
	FItemUnHovere OnItemUnHovered;

private:

	int32 TotalItemCount = 0;

	//상점 아이템 구매 목록
	TArray<FSelectItem> ShopItemArray;

	//플레이어 아이템 목록
	TArray<FSelectItem> PlayerItemArray;

	//아이템 선택 함수
	FSelectItem GetSelectItem();
	//아이템 정보 반환 함수

public:
	void HoverShopItem(int32 ItemNum);

	void UnHoverShopItem();

	void HoverPlayerItem(int32 ItemNum);

	void UnHoverPlayerItem(int32 ItemNum);

	void GetItem(int32 GetItemID);


	//아이템 구매
	//플레이어가 소지중인 인덱스를 눌러서 추가함
	void AddItemByInven(int32 IndexNum);
	//플레이어가 소지중인 인덱스를 눌러서 삭제함
	void ReduceItemByInven(int32 IndexNum);

	//아이템을 좌클릭 했을 때 추가됨
	void AddItem(int32 GetItemID);
	//아이템을 우클릭 했을 떄 추가됨
	void ReduceItem(int32 GetItemID);

	//아이템을 추가할 수 있는 위치를 받음
	int32 GetCanAddItemIndex(int32 GetItemID);
	//아이템을 제거 할 수 있는 위치를 받음
	int32 GetCanReduceItemIndex(int32 GetItemID);
};
