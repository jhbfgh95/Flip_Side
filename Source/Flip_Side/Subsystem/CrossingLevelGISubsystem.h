// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CoinDataTypes.h"
#include "ItemDataTypes.h"
#include "CrossingLevelGISubsystem.generated.h"
/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UCrossingLevelGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	//슬롯 코인 저장하는 배열
	TArray<FCoinTypeStructure> SlotCoinArray;

	//카드 구조체 저장하는 배열
	TArray<int32> CardIDArray;

	//아이템을 저장하는 배열
	TArray<int32> UseableItemIDArray;

	//디폴트 코인
	FCoinTypeStructure DefaultCoin;

	int32 MakedCoinNum;

	//슬롯에 코인을 제작함
	void InitSlots();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	//슬롯에 코인을 제작함
	UFUNCTION(BlueprintCallable)
	void SetSlotCoin(int SlotNum, FCoinTypeStructure CoinStruct);

	UFUNCTION(BlueprintCallable)
	void SetBattleCardID(int32 CardID, int32 CardSlot);

	UFUNCTION(BlueprintCallable)
	void SetBattleUseItemID(int32 UseableItemID, int32 ItemSlot);	
	
	//슬롯에 해당하는 코인을 반환함
	UFUNCTION(BlueprintCallable)
	FCoinTypeStructure GetSlotCoin(int SlotNum);

	UFUNCTION(BlueprintCallable)
	int32 GetMakedCoinNum();
	
	UFUNCTION(BlueprintCallable)
	TArray<int32> GetBattleCardIDs();
	
	UFUNCTION(BlueprintCallable)
	TArray<int32> GetBattleUseItemIDs();	

	//테스트용 코인슬롯에 코인들을 채움 카드 및 소모품도 넣겠음.
	void GenerateTestCoin();

};
