// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "CoinCreateWSubsystem.generated.h"

/**
 * 코인 제작과 관련된 서브시스템
 */
//코인이을 선택 할때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectedCoin, FCoinTypeStructure, CoinInfo, EWeaponClass, CoinClass);

//무기가 변경되었을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectedCoinUpdate, int32, WeaponID, bool, IsFront);

//코인이 클래스가 선택 됬을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoinClassUpdate,EWeaponClass, SelectedClass);

UCLASS()
class FLIP_SIDE_API UCoinCreateWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& World) override;

private:
	//선택된 코인
	FCoinTypeStructure SelectedCoin;

	/*선택된 코인 클래스*/
	EWeaponClass SelectedCoinClass;

	//제작중인 코인이 앞인지
	bool bIsCreateCoinFront;

public:
	//선택된 코인이 변경되었을 때
    FSelectedCoin OnSelectedCoin;
	//선택한 코인의 스탯이 변경되었을 때
	FSelectedCoinUpdate OnSelectedCoinUpdate;
	//코인의 클래스가 변경됬을 때
	FCoinClassUpdate OnCoinClassUpdate;

public:	
	//선택한 코인반환
	FCoinTypeStructure GetSelectCoin();
	//선택한 코인 클래스 반환
	EWeaponClass GetSelectCoinClass();

	//제작할 코인 선택
	void SelectCoin(FCoinTypeStructure SelectCoinInfo, EWeaponClass CoinClass);

	//선택된 코인 면의 무기를 변경
	void ChangeSelectedCoinWeapon(int32 WeaponID);

	//코인의 역할군 선택
	void SetCoinClass(enum EWeaponClass weponClass);


	//코인의 앞뒤를 변경
	void ChangeCoinSide();
	/*현재 제작중인 코인이 앞면인지*/
	bool GetIsCreateCoinFront();
};
