// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes/CardTypes.h"
#include "ShopCardPresenter.generated.h"

/**
 * 
 */
class UUnlockGISubsystem;
class UW_ShopCardMainWidget;
class UShopCardWSubsystem;
class UDataManagerSubsystem;
class UW_ShopCardSlot;
class UW_ShopPlayerCardSlot;
class UW_ShopPlayerSelectedCardSlot;
UCLASS()
class FLIP_SIDE_API UShopCardPresenter : public UObject
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY()
	TObjectPtr<UShopCardWSubsystem> CardSubsystem;

	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManager;
	
	UPROPERTY()
	TObjectPtr<UUnlockGISubsystem> UnlockSubsystem;
	
protected:
	UPROPERTY()
	TObjectPtr<UW_ShopCardMainWidget> ShopCardMainWidget;

public:
	void InitPresenter(UW_ShopCardMainWidget* InShopCardMainWidget, 
		UShopCardWSubsystem* InShopCardSubsystem, UDataManagerSubsystem* InDataManager
	,UUnlockGISubsystem* InUnlockSubsystem);
protected:
	
	TArray<TObjectPtr<UW_ShopCardSlot>> ShopCardSlotViews;

	TArray<TObjectPtr<UW_ShopPlayerCardSlot>> ShopPlayerCardSlotViews;

	TArray<TObjectPtr<UW_ShopPlayerSelectedCardSlot>> ShopPlayerSelectedCardSlotViews;

protected:
	UFUNCTION()
	void BuyCard(UW_ShopCardSlot* BuyCardSlot, int32 CardID);
	
	UFUNCTION()
	void SelectedPlayerCard(UW_ShopPlayerCardSlot* SelectedCardSlot, int32 CardID);

	UFUNCTION()
	void UnselectedPlayerSelectedCard(int32 Idnex);

	UFUNCTION()
	void UnselectedPlayerCard(int32 CardID);

	UFUNCTION()
	void HoveredCard(int32 CardID);
	UFUNCTION()
	void UnhoveredCard();

	UFUNCTION()
	void AddCard(int32 CardId);
protected:
	FCardData GetCardData(int32 GetCardID);

private:
	int32 CurrentHoveredCardID = -1;

	void InitShopCardSlot();
	
	void InitShopPlayerCardSlot();

};
