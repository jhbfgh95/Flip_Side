// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCoinCreate/ShopCreateCoin.h"
#include "Tuto_ShopCreateCoin.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATuto_ShopCreateCoin : public AShopCreateCoin
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	
	class UShopTutorialWSubsystem* TutoSubsystem;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 ExcuteOrder;

private:
	virtual void InteractLeftClick_Implementation() override;
};
