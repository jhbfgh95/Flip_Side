// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCoinManage/CoinSlotLockPanel.h"
#include "Interface/ShopTutorialInterface.h"
#include "Tuto_CoinSlotLockPanel.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATuto_CoinSlotLockPanel : public ACoinSlotLockPanel,public IShopTutorialInterface
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, meta = (AlloPrivateAllow))
	int32 ExecuteOrder;

	class UShopTutorialWSubsystem* TutorialSubsystem;

	
	virtual void InteractLeftClick_Implementation() override;
};
