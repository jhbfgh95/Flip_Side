// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ItemLogicLibrary.generated.h"

class UItem_Action;
UCLASS()
class FLIP_SIDE_API UItemLogicLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void Test_Item(UItem_Action* ItemContext);
};
