// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopItem/PlayerItemInven.h"
#include "Tuto_PlayerItemInven.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATuto_PlayerItemInven : public APlayerItemInven
{
	GENERATED_BODY()

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
