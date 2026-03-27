// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interface/ShopTutorialInterface.h"
#include "ShopTutorialWSubsystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FShopTutorialEvents
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TScriptInterface<IShopTutorialInterface>> TutorialEvents;
};


UCLASS()
class FLIP_SIDE_API UShopTutorialWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
private:
	TArray<FShopTutorialEvents>ReadyTutorialEvents;

public:
	bool SetTutorialEvent(int32 EventOrder, TScriptInterface<IShopTutorialInterface> SetEvent);
	void ExecuteEvents(int32 EventOrder);
};
