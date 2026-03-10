// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShopMouseInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShopMouseInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FLIP_SIDE_API IShopMouseInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InteractLeftClick();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InteractRightClick();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InteractHover();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InteractUnHover();
};
