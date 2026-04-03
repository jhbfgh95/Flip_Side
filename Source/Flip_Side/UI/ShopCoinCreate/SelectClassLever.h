// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlipSide_Enum.h"
#include "LeverActor.h"
#include "Interface/ShopMouseInterface.h"
#include "SelectClassLever.generated.h"

UCLASS()
class FLIP_SIDE_API ASelectClassLever : public ALeverActor,public IShopMouseInterface
{
	GENERATED_BODY()
	
protected:
	ASelectClassLever();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* InteractBox;
private:
	class UCoinCreateWSubsystem* CoinCreateSubsystem;

protected:
	virtual void ExecuteLeverLogic() override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	EWeaponClass LeverClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor DealClassColor;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor TankClassColor;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor UtilClassColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor DefaultColor;

public:
	virtual void InteractLeftClick_Implementation() override;
};
