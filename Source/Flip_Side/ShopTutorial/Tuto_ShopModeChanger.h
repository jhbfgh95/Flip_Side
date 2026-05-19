// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopTutorialInterface.h"
#include "Tuto_ShopModeChanger.generated.h"

UCLASS()
class FLIP_SIDE_API ATuto_ShopModeChanger : public AActor, public IShopTutorialInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATuto_ShopModeChanger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class AGameMode_Shop* TutoGameMode;

	class AShopController_FlipSide* ShopController;

	class UShopTutorialWSubsystem* TutoSubsystem;

	virtual void ExecuteTutorialEvent_Implementation() override;

	virtual void FinishTutorialEvent_Implementation() override;

	int16 EventCount;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TArray<int32> Order;
};
