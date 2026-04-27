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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponClass CoinClass;
};

/*코인 개수가 변경됬을 떄 델리게이트*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCoinCountUpdate , int32 , CoinSlotIndex, int32, CoinCount);

/*코인 슬롯이 변경되었을 떄 델리게이트 위에서 아래로 변경 됬는지 검사*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeCoinSlot);
/*코인 슬롯 해금시 델리게이트*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnlockCoinSlot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHoverWeapon,int32, WeaponID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnHoverWeapon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeSlotCoinSide, bool, IsFront);
/*코안 제작 시작 시 데이터 값을 넘기는 델리게이트*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCoinCreated, int32, CreatedCoinIndex, EWeaponClass, CreateWeaponClass );

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWeapon, int32, WeaponID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeCoinClass, EWeaponClass, ChangeClass);

// WarningNum == 0 같은 무기 앞뒤 / == 1 같은 코인이 슬롯에 존재 / == 2 슬롯 잠김 / ==3 양면에 무기 없음 / == 4 전체 개수 꽉참 / == 5 슬롯 개수 꽉참
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWarningCreate, int32, WarningNum);

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
	/*10개의 코인슬롯 정보*/
	TArray<FShopCoinSlotData> ShopCoinSlotArray;
	/*현재 선택 중인 코인 슬롯 번호*/
	int32 CurrentCoinSlotNum;

/*델리게이트들*/
public:
	FChangeCoinSlot OnCoinSlotChange;
	FCoinCountUpdate OnCoinCountUpdate;
	FUnlockCoinSlot OnUnlockCoinSlot;
	FCoinCreated OnCoinCreated;
	FChangeSlotCoinSide OnChangeSlotCoinSide;
	FSetWeapon OnSetWeapon;
	FWarningCreate OnWarningCreate;
	FHoverWeapon OnHoverWeapon;
	FUnHoverWeapon OnUnHoverWeapon;
	FChangeCoinClass OnChangeCoinClass;
private:
	//코인 개수를 증가 시킬수 있는가?
	bool CanIncreaseCoin(int32 SlotNum);
	//코인 개수를 감소 시킬수 있는가?
	bool CanDecreaseCoin(int32 SlotNum);

	int32 GetSameWeaponInCoinSlot(int32 SlotNum, int32 WeaponID);

	bool IsTrySetSameWeapon(bool IsFront, int32 WeaponID);
public:
	//특정 번호의 코인 슬롯으로 변경
	void ChangeCoinSlotByIndex(int32 SlotNum);
	//현재 코인슬롯을 개방

	void UnlockCurrentCoinSlot();
	void UnlockCoinSlot(int32 SlotNum);
	//현재 코인슬롯이 해금되었는지 반환
	bool GetCurrentCoinUnlock();
	bool GetCoinUnlockByIndex(int32 index);
	//현재 코인슬롯 번호에 해당하는 코인 정보를 가져옴
	FCoinTypeStructure GetSlotCoin(int32 index);
	EWeaponClass GetSlotCoinClass(int32 index);
	int32 GetSlotCoinCount(int32 index);
	//코인 잠금 해제
	void UnlockCoin();

	/*현재 코인슬롯의 코인 개수 증가*/
	void IncreaseSlotCoinCount();
	/*현재 코인슬롯의 코인 개수 감소*/
	void DecreaseSlotCoinCount();

	
	void IncreaseSlotCoinCount(int32 SlotNum);
	void DecreaseSlotCoinCount(int32 SlotNum);


	//슬롯 번호에 해당하는 코인 초기화
	void ResetCoin(int32 SlotNum);
	void ResetCoinSide(int32 SlotNum, bool IsFront);

	//현재 코인슬롯의 코인 정보를 가져옴
	FCoinTypeStructure GetCurrentSlotCoin();
	//현재 코인슬롯의 클래스 가져옴
	EWeaponClass GetCurrentSlotCoinClass();

	//현재 코인슬롯의 코인을 할당
	void SetSlotCoin(FCoinTypeStructure SetCoinInfo, EWeaponClass CoinClass);

	void ChangeSlotCoinSide(bool IsChangedSideFront);

public:
	int32 GetCurrentCoinCount();
	int32 GetCurrentSlotNum();
	int32 GetTotalCoinCount();


public:
	void SelectCoin(int32 SlotNum);
	
private:
	bool IsCreateCoinFront;
public:	

	//코인에 무기 장착
	void SetWeaponToCoinSide(int32 WeaponID, EWeaponClass WeaponClass);
	//코인의 앞뒤를 변경
	void ChangeCoinSide();
	/*현재 제작중인 코인이 앞면인지*/
	bool GetIsCreateCoinFront();
public:
	int32 GetCurrentCoinWeaponID(bool IsFront);
	void ExecuteWarning(int32 WarningCode);

public:
	void HoverWeapon(int32 WeaponID);
	void UnHoverWeapon();

};
