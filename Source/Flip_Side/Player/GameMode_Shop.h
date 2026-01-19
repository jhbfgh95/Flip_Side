// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_Shop.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AGameMode_Shop : public AGameModeBase
{
	GENERATED_BODY()
	
	//
	public:
	void SetMainMode();
	void SetCheckBossMode();
	void SetCoinCreateMode();
	void SetCardSelectMode();
	void SetBuyItemMode();
	void CheckCanGameStart();
};
