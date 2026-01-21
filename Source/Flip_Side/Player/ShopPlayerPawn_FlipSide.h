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

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector CoinCreateLocation;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector CoinManageLocation;

private:
	UFUNCTION()
	void MoveCoinCreateMode();

	
	UFUNCTION()
	void MoveCoinManageMode();
};
