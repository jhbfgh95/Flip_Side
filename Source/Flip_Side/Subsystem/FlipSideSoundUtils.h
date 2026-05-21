#pragma once

#include "CoreMinimal.h"

class USoundBase;

class FLIP_SIDE_API FFlipSideSoundUtils
{
public:
	static void PlayDefaultClickSound(const UObject* WorldContextObject);
	static void PlayShopBuyClickSound(const UObject* WorldContextObject);
	static void PlayShopSellClickSound(const UObject* WorldContextObject);
	static void PlayBattleItemClickSound(const UObject* WorldContextObject);

private:
	static void PlaySoftSound(const UObject* WorldContextObject, const TSoftObjectPtr<USoundBase>& SoundAsset);
};
