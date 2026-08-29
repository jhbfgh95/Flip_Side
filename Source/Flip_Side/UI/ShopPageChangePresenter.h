#pragma once

#include "CoreMinimal.h"
#include "DataTypes/ShopPageTypes.h"
#include "UObject/NoExportTypes.h"
#include "ShopPageChangePresenter.generated.h"

class AShopPlayerPawn_FlipSide;
class UW_ShopWidgetContainer;
class AShopUISelectRegistry;
UCLASS()
class FLIP_SIDE_API UShopPageChangePresenter : public UObject
{
	GENERATED_BODY()

public:
	void InitPresenter(AShopUISelectRegistry* InShopUISelectRegistry,
		UW_ShopWidgetContainer* InWidgetContainer,
		AShopPlayerPawn_FlipSide* InShopPawn);

protected:
	UPROPERTY()
	TObjectPtr<UW_ShopWidgetContainer> WidgetContainer;

	UPROPERTY()
	TObjectPtr<AShopPlayerPawn_FlipSide> ShopPawn;

	UPROPERTY()
	TObjectPtr<AShopUISelectRegistry> ShopUISelectRegistry;

private:
	UFUNCTION()
	void HandlePageRequested(EShopPage Page);

	UFUNCTION()
	void HandleMoveCompleted(EShopPage Page);

private:
	void InitShopUISelectActor();

	EShopPage PendingPage = EShopPage::Main;
	bool bIsTransitioning = false;

};
