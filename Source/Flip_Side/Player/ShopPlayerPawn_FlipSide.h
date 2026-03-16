// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShopPlayerPawn_FlipSide.generated.h"

class UCameraComponent;

UCLASS(abstract)
class AShopPlayerPawn_FlipSide : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

private:
	class AGameMode_Shop* ShopGameMode;
public:
	AShopPlayerPawn_FlipSide();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector CoinCreateLocation;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector CoinManageLocation;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector SelectCardLocation;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector ShopItemLocation;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector UnlockWeaponLocation;
	
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector ShopMainLocation;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector UnlockCardLocation;
private:
	void SetCameraOrthographic();
	void SetCameraPerspective();

private:
	UFUNCTION()
	void MoveCoinCreateMode();
	
	UFUNCTION()
	void MoveCoinManageMode();

	UFUNCTION()
	void MoveSelectCardMode();

	UFUNCTION()
	void MoveShopItemMode();
	
	UFUNCTION()
	void MoveUnlockWeaponMode();
	
	UFUNCTION()
	void MoveShopMainMode();

	UFUNCTION()
	void MoveUnlockCardMode();
};
