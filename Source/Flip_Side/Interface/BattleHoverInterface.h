// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BattleHoverInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBattleHoverInterface : public UInterface
{
	GENERATED_BODY()
};


class FLIP_SIDE_API IBattleHoverInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnHover();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnUnhover();
};
