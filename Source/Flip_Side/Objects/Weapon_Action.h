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
	
	//받아오기 전에 float로 고쳐서 계산해서 주세요~ StatComponent님
	float FinalAttackPoint = 0.f; 

	float FinalBehaviorPoint = 0.f;

public:
	/*Set은 CoinAcionManagementWSubsystem이*/
	virtual void SetFinalAttackPoint(const float AttackPoint);

	virtual void SetFinalBehaviorPoint(const float BehaviorPoint);

	/*Get은 CoinActionStaticLibrary에서 */

	virtual float GetFinalAttackPoint() const;

	virtual float GetFinalBehaviorPoint() const;

	virtual void ExecuteAction() override;
};
