#pragma once

#include "CoreMinimal.h"
#include "ShopPageTypes.generated.h"

UENUM(BlueprintType)
enum class EShopPage : uint8
{
	None = 255,
	Main = 0,
	Coin,
	Item,
	Card,
	UnlockWeapon,
	Boss,
	GameStart
};
