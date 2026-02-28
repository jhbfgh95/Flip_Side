// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponLogicLibrary.generated.h"

class UWeapon_Action;

UCLASS()
class FLIP_SIDE_API UWeaponLogicLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void Test_Logic(UWeapon_Action* WeaponContext);
};
