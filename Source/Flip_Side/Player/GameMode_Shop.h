// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_Shop.generated.h"

/**
 * 
 */
//코인이 클래스가 선택 됬을 때 델리게이트

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMainMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCoinCreateMode);

UCLASS()
class FLIP_SIDE_API AGameMode_Shop : public AGameModeBase
{
	GENERATED_BODY()
	
	//
public:
	FMainMode OnMainMode;
	FCoinCreateMode OnCoinCreateMode;

public:
	void SetMainMode();
	void SetCheckBossMode();

	UFUNCTION(BlueprintCallAble)
	void SetCoinCreateMode();

	void SetCardSelectMode();
	void SetBuyItemMode();
	void CheckCanGameStart();
};
