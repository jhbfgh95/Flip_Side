// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopUnlockWeapon/UnlockWeaponLever.h"
#include "Interface/ShopTutorialInterface.h"
#include "Tuto_UnlockWeaponLever.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATuto_UnlockWeaponLever : public AUnlockWeaponLever, public IShopTutorialInterface
{
	GENERATED_BODY()

	protected:
ATuto_UnlockWeaponLever();

private:
	virtual void BeginPlay() override;
	
private:
	class UShopTutorialWSubsystem* TutorialSubsytem;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 Order = -1;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 NextOrder = -1;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsExecuteNextOrder = true;
	
private:

	virtual void ExecuteTutorialEvent_Implementation() override;
	
	virtual void ExecuteLeverLogic() override;

};
