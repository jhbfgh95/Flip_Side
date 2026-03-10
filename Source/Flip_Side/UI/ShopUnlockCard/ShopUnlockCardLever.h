// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LeverActor.h"
#include "Interface/ShopMouseInterface.h"
#include "ShopUnlockCardLever.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AShopUnlockCardLever : public ALeverActor, public IShopMouseInterface
{
	GENERATED_BODY()

	AShopUnlockCardLever();
private:
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess="true"))
	class UBoxComponent* InteractBox;
private:
	class UShopUnlockCardWSubsystem* UnlockCardSubsystem;
	
protected:
	virtual void BeginPlay() override;
public:
	virtual void ExecuteLeverLogic() override;

	virtual void InteractLeftClick_Implementation() override;

};
