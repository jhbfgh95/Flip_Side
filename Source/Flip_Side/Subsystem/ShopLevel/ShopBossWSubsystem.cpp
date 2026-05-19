#include "Subsystem/ShopLevel/ShopBossWSubsystem.h"

bool UShopBossWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	Super::ShouldCreateSubsystem(Outer);

	UWorld* World = Cast<UWorld>(Outer);
	if (!World || !World->IsGameWorld())
		return false;

	const FString MapName = World->GetMapName();
	return MapName.Contains(TEXT("L_ShopLevel")) || MapName.Contains(TEXT("L_Tutorial_Shop_Level"))|| MapName.Contains(TEXT("L_Tutorial_TutoShop_Level"));
}
