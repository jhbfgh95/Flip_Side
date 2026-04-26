// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CardTypes.h"
#include "ShopCardWSubsystem.generated.h"
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnSelectCard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnlockSelectCard, int32, CardId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCardChanged, FCardData, CardInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectCard, FCardData, CardInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangePlayerCard);

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

private:

	TArray<FCardData> UnlockCardList;

	TArray<FCardData> TotalCardList;

	TArray<FCardData> ShopCardList;

	TArray<FCardData> PlayerCardList;

	int32 CurrentCardListNum;

	FCardData DefaultCard;

	FCardData CurrentSelectCard;


private:
	bool CanSelectCard();

	int32 CanSelectPlayerIndex();

	UFUNCTION()
	void SetUnlockCard();
	
	UFUNCTION()
	void AddCardListToUnlockCard(int32 UnlockCardID);



public:

	FCardChanged OnCardChanged;
	FSelectCard OnSelectCard;
	FUnSelectCard OnUnSelectCard;
	FUnlockSelectCard OnUnlockCard;
	FChangePlayerCard OnChangePlayerCard;
public:
	int32 PlayerSelectCard;

	int32 GetPlayerCardID(int32 index);

public:
	
	TArray<FCardData> GetCardListArray();

	void SelectCard(FCardData CardData);

	void UnSelectCard();

	void UnlockCard();

	void SelectPlayerCard();

	void UnSelectPlayerCard(FCardData CardData);

	TArray<FCardData> GetPlayerCardList();
};
