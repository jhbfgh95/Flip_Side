// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CoinDataTypes.h"
#include "CoinDataTypes.h"
#include "ItemDataTypes.h"
#include "CoinGameInstance.generated.h"
/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UCoinGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

	private:
	//슬롯 코인 저장하는 배열
	TArray<FCoinTypeStructure> SlotCoinArray;

	//카드 구조체 저장하는 배열
	//TArray<> CardArray;

	//아이템을 저장하는 배열
	TArray<FItemData> ItemArray;


	//디폴트 코인
	FCoinTypeStructure DefaultCoin;

	
	//슬롯에 코인을 제작함
	void InitCointSlot();


	protected:

	virtual void Init() override;



	public:

	//슬롯에 코인을 제작함
	UFUNCTION(BlueprintCallable)
	void SetSlotCoin(int SlotNum, FCoinTypeStructure CoinStruct);
	
	//슬롯에 해당하는 코인을 반환함
	UFUNCTION(BlueprintCallable)
	FCoinTypeStructure GetSlotCoin(int SlotNum);

	
	//테스트용 코인슬롯에 코인들을 채움
	void GenerateTestCoin();

};
