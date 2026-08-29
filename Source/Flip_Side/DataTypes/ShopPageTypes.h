#pragma once

#include "CoreMinimal.h"
#include "ShopPageTypes.generated.h"

UENUM(BlueprintType)
enum class EShopPage : uint8
{
	Main,
	Coin,
	Item,
	Card,
	UnlockWeapon,
	Boss
};
