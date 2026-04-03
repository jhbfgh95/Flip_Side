// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopUnlockWeapon/UnlockWeaponClassLever.h"
#include "Tuto_UnlockWeaponClassLever.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATuto_UnlockWeaponClassLever : public AUnlockWeaponClassLever
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
private:
	class UShopTutorialWSubsystem* TutorialSubsytem;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 NextOrder = -1;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsExecuteNextOrder = true;
	
private:

	virtual void ExecuteLeverLogic() override;
};
