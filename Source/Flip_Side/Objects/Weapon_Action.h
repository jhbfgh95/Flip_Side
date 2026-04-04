// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ActionBase.h"
#include "WeaponDataTypes.h"
#include "Weapon_Action.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UWeapon_Action : public UActionBase
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Status")
	int32 FinalAttackPoint = 0; 

	UPROPERTY(VisibleAnywhere, Category = "Weapon | Status")
	int32 FinalBehaviorPoint = 0;

	UPROPERTY()
	class ACoinActor* CasterCoin = nullptr;

	UPROPERTY()
	class AGridActor* TargetGrid = nullptr;

	//굳이 필요없지만 나중에 버프등의 아이콘 넣을 수도 있으니까
	UPROPERTY()
	FFaceData WeaponData;

public:
	/*Set은 CoinAcionManagementWSubsystem이*/
	virtual void SetFinalAttackPoint(const int32 AttackPoint);

	virtual void SetFinalBehaviorPoint(const int32 BehaviorPoint);

	virtual void SetSingleCellTargetCoin(ACoinActor* TargetCoin);

	virtual void SetGridForAction(class AGridActor* targetGrid);

	void SetWeaponData();

	void SetCasterCoin(ACoinActor* InCaster) { CasterCoin = InCaster; }
    ACoinActor* GetCasterCoin() const { return CasterCoin; }

	AGridActor* GetTargetGrid() const { return TargetGrid; }

	/*Get은 CoinActionStaticLibrary에서 */

	virtual int32 GetFinalAttackPoint() const;

	virtual int32 GetFinalBehaviorPoint() const;

	virtual void ExecuteAction() override;
};
