// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopItem/ShopUseableItemActor.h"
#include "Tuto_ShopUseableItemActor.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATuto_ShopUseableItemActor : public AShopUseableItemActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
private:
	class UShopTutorialWSubsystem* TutorialSubsytem;

private:
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 NextOrder = -1;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsExecuteNextOrder = true;
	
private:

	void ExecuteNextTutorial();
	virtual void InteractLeftClick_Implementation() override;
};
