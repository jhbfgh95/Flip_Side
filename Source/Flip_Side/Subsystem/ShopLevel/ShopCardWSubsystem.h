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
	virtual void OnWorldBeginPlay(UWorld& World) override;
	virtual void Deinitialize() override;
private:
	class UDataManagerSubsystem* DM;
	
	class UUnlockGISubsystem* UnlockSubsystem;
private:
	TArray<FCardData> UnlockCardList;

	TArray<FCardData> PlayerCardList;

	int32 CurrentCardListNum;
	FCardData DefaultCard;

private:
	bool CanSelectCard();

	UFUNCTION()
	void SetUnlockCard();
	
	UFUNCTION()
	void AddCardListToUnlockCard(int32 UnlockCardID);
public:
	FCardChanged OnCardChanged;
	FCardSelected OnCardSelected;
	
	void SetNextCard();

	void SetPreviousCard();

	int32 PlayerSelectCard;

	void SelectCard();

	FCardData GetCurrentCard();

	void RemoveHandCard(int32 HandIndex);

	int32 GetPlayerCardID(int32 index);
};
