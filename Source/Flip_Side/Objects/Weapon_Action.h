// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ActionBase.h"
#include "Weapon_Action.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UWeapon_Action : public UActionBase
{
	GENERATED_BODY()
	
	int32 FinalAttackPoint = 0; 

	int32 FinalBehaviorPoint = 0;

public:
	/*Set은 CoinAcionManagementWSubsystem이*/
	virtual void SetFinalAttackPoint(const int32 AttackPoint);

	virtual void SetFinalBehaviorPoint(const int32 BehaviorPoint);

	/*Get은 CoinActionStaticLibrary에서 */

	virtual int32 GetFinalAttackPoint() const;

	virtual int32 GetFinalBehaviorPoint() const;

	virtual void ExecuteAction() override;
};
