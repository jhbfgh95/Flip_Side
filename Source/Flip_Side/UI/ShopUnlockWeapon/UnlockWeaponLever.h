// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LeverActor.h"
#include "Interface/ShopMouseInterface.h"
#include "UnlockWeaponLever.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AUnlockWeaponLever : public ALeverActor, public IShopMouseInterface
{
	GENERATED_BODY()
protected:
	AUnlockWeaponLever();
private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	class UBoxComponent* InteractBox;
private:
	class UShopUnlockWeaponWSubsystem* UnlockWeaponSubsystem;
	
	protected:
	virtual void BeginPlay() override;
public:
	virtual void ExecuteLeverLogic() override;

	virtual void InteractLeftClick_Implementation() override;
};
