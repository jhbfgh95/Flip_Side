// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/ShopController_FlipSide.h"
#include "ShopTutoController_FlipSide.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AShopTutoController_FlipSide : public AShopController_FlipSide
{
	GENERATED_BODY()

protected:
	//상점 메인 UI
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> NonTutoShopMainWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> NonTutoShopModeWidgetClass;

public:
	void SetNonTutorialWidget();
};
