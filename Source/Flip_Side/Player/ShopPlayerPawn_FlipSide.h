// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/ShopPageTypes.h"
#include "GameFramework/Pawn.h"
#include "ShopPlayerPawn_FlipSide.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopPawnMoveCompleted, EShopPage, Page);

UCLASS(abstract)
class AShopPlayerPawn_FlipSide : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

public:
	AShopPlayerPawn_FlipSide();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Shop|Movement")
	FOnShopPawnMoveCompleted OnShopPawnMoveCompleted;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FTransform CoinTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FTransform ShopItemTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FTransform UnlockWeaponTransform = FTransform::Identity;
	
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FTransform ShopMainTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FTransform CardTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FTransform CheckBossInfoTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.01", AllowPrivateAccess = "true"))
	float LocationInterpSpeed = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.01", AllowPrivateAccess = "true"))
	float RotationInterpSpeed = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float LocationCompletionTolerance = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float RotationCompletionTolerance = 0.1f;

	FTransform TargetTransform = FTransform::Identity;
	EShopPage TargetShopPage = EShopPage::Main;
	bool bIsMoving = false;
	bool bBroadcastOnMoveCompleted = false;

private:
	void SetCameraOrthographic();
	void SetCameraPerspective();
	void StartMove(const FTransform& NewTargetTransform);
	void StartShopPageMove(const FTransform& NewTargetTransform, EShopPage Page);

public:
	void MoveToShopPage(EShopPage Page);
	void MoveShopMainMode();
};
