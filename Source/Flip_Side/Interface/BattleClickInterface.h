// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BattleClickInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBattleClickInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FLIP_SIDE_API IBattleClickInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnClicked();

};
