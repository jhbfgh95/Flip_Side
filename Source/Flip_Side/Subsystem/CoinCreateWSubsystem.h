// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "CoinCreateWSubsystem.generated.h"

/**
 * 
 */
//선택된 코인이 변경되었을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectedCoinChanged, FCoinTypeStructure, SelectCoinInfo);

//코인이 변경되었을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectedCoinUpdate, int32, WeaponID);


//코인이 클래스가 선택 됬을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoinClassUpdate,EWeaponClass, SelectedClass);

UCLASS()
class FLIP_SIDE_API UCoinCreateWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	
public:
	//선택된 코인
	FCoinTypeStructure SelectedCoin;
	//선택된 코인이 변경되었을 때
    FSelectedCoinChanged OnSelectedCoinChanged;
	//선택한 코인의 스탯이 변경되었을 때
	FSelectedCoinUpdate OnSelectedCoinUpdate;
	//코인의 클래스가 변경됬을 때
	FCoinClassUpdate OnCoinClassUpdate;

	//코인반환
	FCoinTypeStructure GetSelectCoin();

	//코인 선택
	void SelectCoin(FCoinTypeStructure SelectCoinInfo);
	//선택된 코인 면의 ID를 변경
	void ChangeSelectedCoinWeapon(int32 WeaponID);
	//코인의 역할군 선택
	void SetCoinClass(enum EWeaponClass weponClass);
	

	//제작중인 코인이 앞인지
	bool bIsCreateCoinFront;
	//코인의 앞뒤를 변경
	void ChangeCoinSide();

	bool GetIsCreateCoinFront();
	
};
