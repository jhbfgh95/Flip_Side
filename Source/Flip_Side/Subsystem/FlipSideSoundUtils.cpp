#include "Subsystem/FlipSideSoundUtils.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Subsystem/FlipSideDevloperSettings.h"

void FFlipSideSoundUtils::PlayDefaultClickSound(const UObject* WorldContextObject)
{
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!Settings) return;

	PlaySoftSound(WorldContextObject, Settings->DefaultClickSFX);
}

void FFlipSideSoundUtils::PlayShopBuyClickSound(const UObject* WorldContextObject)
{
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!Settings) return;

	PlaySoftSound(WorldContextObject, Settings->ShopBuyClickSFX);
}

void FFlipSideSoundUtils::PlayShopSellClickSound(const UObject* WorldContextObject)
{
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!Settings) return;

	PlaySoftSound(WorldContextObject, Settings->ShopSellClickSFX);
}

void FFlipSideSoundUtils::PlayBattleItemClickSound(const UObject* WorldContextObject)
{
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!Settings) return;

	PlaySoftSound(WorldContextObject, Settings->BattleItemClickSFX);
}

void FFlipSideSoundUtils::PlaySoftSound(const UObject* WorldContextObject, const TSoftObjectPtr<USoundBase>& SoundAsset)
{
	if (!WorldContextObject || SoundAsset.IsNull())
	{
		return;
	}

	if (USoundBase* Sound = SoundAsset.LoadSynchronous())
	{
		UGameplayStatics::PlaySound2D(WorldContextObject, Sound);
	}
}
