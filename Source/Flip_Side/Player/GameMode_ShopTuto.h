// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/GameMode_Shop.h"
#include "GameMode_ShopTuto.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AGameMode_ShopTuto : public AGameMode_Shop
{
	GENERATED_BODY()
	
public:
	virtual void ChangeBattleLevel() override;
};
