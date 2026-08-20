// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes/ItemDataTypes.h"
#include "ShopItemPresenter.generated.h"

/**
 * 
 */
class UShopItemWSubsystem;
class UW_ShopItemSlot;
class UW_ShopPlayerItemSlot;
class UDataManagerSubsystem;
class UW_ShopItemSlotContainer;
class UW_ShopPlayerItemSlotContainer;
class UW_ShopItemWidget;
USTRUCT(BlueprintType)
struct FShopItemWidgets
{
    GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UW_ShopItemSlotContainer> ShopItemSlotContainer;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UW_ShopPlayerItemSlotContainer> ShopPlayerItemSlotContainer;
	
};

UCLASS()
class FLIP_SIDE_API UShopItemPresenter : public UObject
{
	GENERATED_BODY()
	
public:
    void InitPresenter(UW_ShopItemWidget* InShopItemWidget, UShopItemWSubsystem* InItemSubsystem, UDataManagerSubsystem* InDataManager);

protected:
	UPROPERTY()
	TObjectPtr<UShopItemWSubsystem> ItemSubsystem;

	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManager;

protected:
	UPROPERTY()
	TObjectPtr<UW_ShopItemWidget> ShopItemWidget;

	UPROPERTY()
	TArray<TObjectPtr<UW_ShopItemSlot>> ShopItemSlotViews;

	UPROPERTY()
	TArray<TObjectPtr<UW_ShopPlayerItemSlot>> ShopPlayerItemSlotViews;

protected:

	UFUNCTION()
	void BuyItem(int32 ItemID, int32 Count);
	UFUNCTION()
	void SellItem(int32 Index, int32 ItemID, int32 Count);
	UFUNCTION()
	void HoveredItemSlot(int32 ItemID);
	UFUNCTION()
	void UnhoveredItemSlot();
	UFUNCTION()
	void HoveredPlayerItemSlot(int32 ItemIndex);
	UFUNCTION()
	void UnhoveredPlayerItemSlot();

	UFUNCTION()
	void SetPlayerItemSlot(int32 SetSlotIndex);

	UFUNCTION()
	void CheckCanItemBuy(UW_ShopItemSlot* BuyItemSlot, int32 BuyItemData, int32 Count);

private:
	void SetShopSlotItemViews();
	void SetPlayerSlotItemViews();
	void SetSelectedItemImage(const FItemData& ItemData);

	FItemData GetItemData(int32 ID);
};
