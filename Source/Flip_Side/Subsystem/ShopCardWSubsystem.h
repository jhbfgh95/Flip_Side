// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CardTypes.h"
#include "ShopCardWSubsystem.generated.h"
/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCardChanged, FCardData, CardInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCardSelected, int32, SelectCardIndex, FCardData, CardInfo);
UCLASS()
class FLIP_SIDE_API UShopCardWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	TArray<FCardData> CardList;

	TArray<FCardData> PlayerCardList;

	int32 CurrentCardListNum;


private:
	bool CanSelectCard();
public:
	FCardChanged OnCardChanged;
	FCardSelected OnCardSelected;
	
	void SetNextCard();

	void SetPreviousCard();

	int32 PlayerSelectCard;

	void SelectCard();

	FCardData GetCurrentCard();

	void RemoveHandCard(int32 HandIndex);
};
