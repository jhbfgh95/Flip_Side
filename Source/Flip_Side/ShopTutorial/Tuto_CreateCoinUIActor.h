// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCoinCreate/CreateCoinUIActor.h"
#include "Tuto_CreateCoinUIActor.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATuto_CreateCoinUIActor : public ACreateCoinUIActor
{
	GENERATED_BODY()
private:
	virtual void BeginPlay() override;
	
private:
	class UShopTutorialWSubsystem* TutorialSubsytem;

private:
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 NextOrder = -1;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsExecuteNextOrder = true;
	
private:
	virtual void InteractLeftClick_Implementation() override;
};
