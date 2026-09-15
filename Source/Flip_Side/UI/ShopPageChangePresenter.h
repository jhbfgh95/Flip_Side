#pragma once

#include "CoreMinimal.h"
#include "DataTypes/ShopPageTypes.h"
#include "TimerManager.h"
#include "UObject/NoExportTypes.h"
#include "ShopPageChangePresenter.generated.h"

class AShopPlayerPawn_FlipSide;
class UW_ShopWidgetContainer;
class AShopUISelectRegistry;
class AShopUISelectActor;
class ULightComponent;

struct FShopLightFadeTarget
{
	TWeakObjectPtr<ULightComponent> Light;
	float TargetIntensity = 0.0f;
};

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

	UPROPERTY()
	TArray<TObjectPtr<AShopUISelectActor>> SelectActors;

private:
	UFUNCTION()
	void HandlePageRequested(EShopPage Page);

	UFUNCTION()
	void HandleMoveCompleted(EShopPage Page);

	UFUNCTION()
	void SetLight(EShopPage Page);

	UFUNCTION()
	void SetLightMain();
	
private:
	void InitShopUISelectActor();

	void SetShopUISelectActorsEnabled(bool bEnabled);
	void FadeLightTo(ULightComponent* Light, float TargetIntensity);
	void UpdateLightFade();

	EShopPage PendingPage = EShopPage::Main;
	bool bIsTransitioning = false;

	FTimerHandle LightFadeTimer;
	TArray<FShopLightFadeTarget> LightFadeTargets;

	UPROPERTY(EditDefaultsOnly, Category = "Shop|Light", meta = (ClampMin = "0.1"))
	float LightFadeInterpSpeed = 4.0f;

};
