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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopPageMoveCompleted, EShopPage, CompletedPage);

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

	UPROPERTY(BlueprintAssignable)
	FOnShopPageMoveCompleted OnPageMoveCompleted;

	// Main은 모든 선택 액터를, 그 외 페이지는 대응하는 선택 액터 하나만 클릭 가능하게 설정합니다.
	void SetShopUISelectActorsEnabledForPage(EShopPage Page);

	UFUNCTION()
	void HandlePageRequested(EShopPage Page);

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
