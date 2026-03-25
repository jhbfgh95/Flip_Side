#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BattleRightClickInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBattleRightClickInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FLIP_SIDE_API IBattleRightClickInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRightClicked();
};
