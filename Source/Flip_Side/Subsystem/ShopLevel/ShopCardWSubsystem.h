// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CardTypes.h"
#include "ShopCardWSubsystem.generated.h"
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnlockSelectCard, int32, CardId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCardChanged, FCardData, CardInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangePlayerCard);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectPlayerCard, FCardData, CardInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUnSelectPlayerCard, FCardData, CardInfo, int32, CardIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnselectPlayerCard);

//0.이미 장착한 카드 //1. 장착 안되있음 //2.카드가 꽉차있음
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShopCardWarning, int32, WarningNum);

UCLASS()
class FLIP_SIDE_API UShopCardWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& World) override;
	virtual void Deinitialize() override;
private:
	class UDataManagerSubsystem* DM;
	class UUnlockGISubsystem* UnlockSubsystem;
	class UMoneyGISubsystem* MoneySubsystem;


private:

	TArray<FCardData> UnlockCardList;

	TArray<FCardData> TotalCardList;

	TArray<FCardData> ShopCardList;

	TArray<FCardData> PlayerCardList;

	int32 CurrentCardListNum;

	FCardData DefaultCard;

private:
	int32 CanSelectPlayerIndex();

	UFUNCTION()
	void SetUnlockCard();
	
	UFUNCTION()
	void AddCardListToUnlockCard(int32 UnlockCardID);

	int32 GetSelectCardListContainIndex(int32 CardID);

public:
	FSelectPlayerCard OnSelectPlayerCard;
	FCardChanged OnCardChanged;
	FUnlockSelectCard OnUnlockCard;
	FChangePlayerCard OnChangePlayerCard;

	FUnSelectPlayerCard OnUnSelectPlayerCard;

	FUnselectPlayerCard OnUnselectPlayerCard;

	FShopCardWarning OnShopCardWarning;

public:
	int32 PlayerSelectCard;

	int32 GetPlayerCardID(int32 index);

public:
	
	TArray<FCardData> GetCardListArray();

	int32 SelectPlayerCard(FCardData CardData);

	bool UnSelectPlayerCard(int32 RemoveIndex);

	TArray<FCardData> GetPlayerCardList();

	FCardData GetPlayerCard(int32 Index);

	bool CheckPlayerHaveCard(int32 CardID);

	int32 GetPlayerCardIndexByID(int32 CardID);

	void WarningShopCard(int32 WarningNum);

	bool BuyCard(FCardData BuyCardData);

	int32 GetPlayerSelectCardCount();


};
