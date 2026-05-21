#include "Subsystem/ShopLevel/ShopSoundManagerWSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/FlipSideDevloperSettings.h"

bool UShopSoundManagerWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	if (!World)
	{
		return false;
	}

	const FString MapName = World->GetMapName();
	return MapName.Contains(TEXT("L_ShopLevel"))
		|| MapName.Contains(TEXT("L_Tutorial_Shop_Level"))
		|| MapName.Contains(TEXT("L_Tutorial_TutoShop_Level"));
}

void UShopSoundManagerWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!Settings || Settings->ShopBGM.IsNull())
	{
		return;
	}

	if (USoundBase* BGM = Settings->ShopBGM.LoadSynchronous())
	{
		BGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), BGM, 1.f, 1.f, 0.f, nullptr, true);
	}
}
