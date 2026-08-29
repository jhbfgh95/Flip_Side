// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopUISelectRegistry.generated.h"

class AShopUISelectActor;

UCLASS()
class FLIP_SIDE_API AShopUISelectRegistry : public AActor
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(EditInstanceOnly, Category = "Shop UI Select Registry", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AShopUISelectActor> CoinUISelectActor;

	UPROPERTY(EditInstanceOnly, Category = "Shop UI Select Registry", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AShopUISelectActor> ItemUISelectActor;

	UPROPERTY(EditInstanceOnly, Category = "Shop UI Select Registry", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AShopUISelectActor> CardUISelectActor;

	UPROPERTY(EditInstanceOnly, Category = "Shop UI Select Registry", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AShopUISelectActor> WeaponUISelectActor;

	UPROPERTY(EditInstanceOnly, Category = "Shop UI Select Registry", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AShopUISelectActor> BossUISelectActor;

public:
	AShopUISelectActor* GetCoinUISelectActor() const { return CoinUISelectActor.Get(); }
	AShopUISelectActor* GetItemUISelectActor() const { return ItemUISelectActor.Get(); }
	AShopUISelectActor* GetCardUISelectActor() const { return CardUISelectActor.Get(); }
	AShopUISelectActor* GetWeaponUISelectActor() const { return WeaponUISelectActor.Get(); }
	AShopUISelectActor* GetBossUISelectActor() const { return BossUISelectActor.Get(); }
};
