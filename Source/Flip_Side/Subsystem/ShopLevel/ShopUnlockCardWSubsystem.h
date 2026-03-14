// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CardTypes.h"
#include "ShopUnlockCardWSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangelockCardsLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangelockCardsRight);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectUnlockCard, int32, SelectCardDataID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnSelectUnlockCard, int32, SelectCardDataID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnlockSelectCard, int32, SelectCardDataID);
UCLASS()
class FLIP_SIDE_API UShopUnlockCardWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& World) override;
public:

	FChangelockCardsLeft OnChangelockCardsLeft;
	FChangelockCardsRight OnChangelockCardsRight;
	FSelectUnlockCard OnSelectUnlockCard;
	FUnlockSelectCard OnUnlockSelectCard;
	FUnSelectUnlockCard OnUnSelectUnlockCard;

private:
	class UUnlockGISubsystem* UnlockSubsystem;
	class UDataManagerSubsystem* DataManager;
private:

	TArray<FCardData> CardList;
	FCardData DefaultCardData;
	int32 CurrentSelectCardID;
private:
	int32 CurrentPage = 0;
	int32 CurrentSelectCardNum = -1;
protected:
	void InitUnlockCard();

public:
	FCardData GetCardDataByIndex(int32 Index);

	FCardData GetCurrnetPageCardData(int32 Index);

	bool UnlockCard();

	void ChangeCardsLeft();
	void ChangeCardsRight();

	void SelectCard(int32 SelectCardID);
	void UnSelectCard();
};
