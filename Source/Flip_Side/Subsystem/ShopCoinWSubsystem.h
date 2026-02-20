// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "ShopCoinWSubsystem.generated.h"

#define MAX_TOTAL_COIN 30
/**
 * 
 */
USTRUCT(BlueprintType)
struct FShopCoinSlotData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool IsUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FCoinTypeStructure CoinData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponClass CoinClass;
};




//코인이 클래스가 선택 됬을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoinCountUpdate , int32 , CountNum);

//코인 슬롯 변경델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeCoinSlot);

//코인슬롯 해금 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnlockCoinSlot);
UCLASS()
class FLIP_SIDE_API UShopCoinWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
private:
	//총 코인 개수
	int32 TotalCoinCount;
	TArray<FShopCoinSlotData> ShopCoinSlotArray;
	//void InitCoinArray();
private:

	//선택한 코인 번호
	int32 CurrentCoinSlotNum;

public:
	//코인 잠금 해제
	void UnlockCoin();
	//코인 설명 출력
	//설명 코인 앞뒤 변경
	//
public:
	//델리게이트
	//코인슬롯이 변경되었을 때
	FChangeCoinSlot OnCoinSlotChange;
	//코인개수가 업데이트 됬을 때
	FCoinCountUpdate OnCoinCountUpdate;
	//코인슬롯을 해금 했을 때
	FUnlockCoinSlot OnUnlockCoinSlot;

public:
	//코인슬롯을 증가시키는 방향으로 변경
	void ChangeCoinSlotRight();
	//코인슬롯을 감소 시키는 방향으로 변경
	void ChangeCoinSlotLeft();
	//코인슬롯을 감소 시키는 방향으로 변경
	void ChangeCoinSlotByIndex(int32 SlotNum);
	//현재 코인슬롯을 개방
	void UnlockCurrentCoinSlot();
	//
	bool GetCurrentCoinUnlock();

	FCoinTypeStructure GetSlotCoin(int32 index);
public:
	bool CanIncreaseCoin(int32 SlotNum);
	bool CanDecreaseCoin(int32 SlotNum);

	void IncreaseSlotCoinCount();
	void DecreaseSlotCoinCount();
	void ResetCoin(int32 SlotNum);


	FCoinTypeStructure GetCurrentSlotCoin();

	EWeaponClass GetCurrentSlotCoinClass();

	void SetSlotCoin(FCoinTypeStructure SetCoinInfo, EWeaponClass CoinClass);
};
