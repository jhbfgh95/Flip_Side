// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "ShopCoinWSubsystem.generated.h"

#define MAX_TOTAL_COIN 30
/**
 * 코인 슬롯 관리 서브 시스템
 * 기능 
 * 1.코인 슬롯 해금 
 * 2. 개수 증감
 */

/*코인 슬롯 구조체 
서브 오브젝트 전용  1. 해금 됬는지, 코인 데이터, 코인 클래스*/
USTRUCT(BlueprintType)
struct FShopCoinSlotData
{
    GENERATED_BODY()
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool IsUnlock = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FCoinTypeStructure CoinData;
};

// WarningNum == 0 같은 무기 앞뒤 / == 1 같은 코인이 슬롯에 존재 / == 2 슬롯 잠김 / ==3 양면에 무기 없음 / == 4 전체 개수 꽉참 / == 5 슬롯 개수 꽉참
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWarningCreate, int32, WarningNum);

class UDataManagerSubsystem;
UCLASS()
class FLIP_SIDE_API UShopCoinWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	class UMoneyGISubsystem* MoneySubsystem;
	class UCrossingLevelGISubsystem* CrossLevelSubsystem;
	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManager;
private:
	//총 코인 개수
	int32 TotalCoinCount;

	/*10개의 코인슬롯 정보*/
	TArray<FShopCoinSlotData> ShopCoinSlotArray;

	int32 LastUnlockCoinSlotIndex = -1;
/*델리게이트들*/
public:
	FWarningCreate OnWarningCreate;

private:
	void InitCoinSetting();

/* 코인 슬롯 관련*/
public:
	//현재 코인슬롯을 개방
	bool BuyCoinSlot(int32 BuySlotLevel);

	bool SellCoinSlot(int32 BuySlotLevel);

	bool GetCoinSlotUnlock(int32 Index);

	void IncreaseCoinSlotCoin(int32 SlotIndex, int32 Amount);

	void DecreaseCoinSlotCoin(int32 SlotIndex, int32 Amount);

	void ResetCoinSlot(int32 SlotIndex);

/*Get*/
	FCoinTypeStructure GetCoinSlotCoinType(int32 SlotIndex);

	int32 GetUnlockCoinSlotCount();

	bool GetIsCoinSlotUnlockByIndex(int32 index);

	FCoinTypeStructure GetSlotCoin(int32 index);

private:

	bool CanIncreaseCoin(int32 SlotIndex,int32 Amount);

	bool CanDecreaseCoin(int32 SlotIndex, int32 Amount);

	int32 GetSameWeaponCoinSlotIndex(int32 CompareSlotIndex, int32 FrontWeaponID, int32 BackWeaponID);

	bool HasSameWeaponOppositeSide(bool IsFront, int32 SlotIndex, int32 WeaponID);

/* 코인 무기 관련*/
public:

	bool TrySetCoinSlotFrontWeapon(int32 SlotIndex, int32 WeaponID);

	bool TrySetCoinSlotBackWeapon(int32 SlotIndex, int32 WeaponID);




/*그 외 */
public:

	bool MoveCoinSlot(int32 SourceIndex, int32 TargetIndex);
	void ExecuteWarning(int32 WarningCode);
	bool GetIsCoinEmpty();
public:

	int32 GetTotalCoinCount();
	int32 GetUnlockSlotLastIndex();
};
