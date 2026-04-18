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

	//보호막 융해
	static void MeltShieldPotion_Logic(UItem_Action* ItemContext);

	//이것도 나중에 피의 갈망
	static void BloodPotion_Logic(UItem_Action* ItemContext);

	//정화
	static void CleanserPotion_Logic(UItem_Action* ItemContext);

	//위상변화
	static void PhaseChangePotion_Logic(UItem_Action* ItemContext);

	//사방팔방 나중에
	static void EverwherePotion_Logic(UItem_Action* ItemContext);

	//융기 물약
	static void WallPotion_Logic(UItem_Action* ItemContext);
};
