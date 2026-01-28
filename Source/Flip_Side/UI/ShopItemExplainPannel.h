// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ExplainPannel.h"
#include "ShopItemExplainPannel.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AShopItemExplainPannel : public AExplainPannel
{
	GENERATED_BODY()
	


	private:
	class UShopItemWSubsystem* ShopItemSubsystem;

	protected:
	virtual void BeginPlay();
	virtual void GetExplainByID(int32 ID) override;

	UFUNCTION()
	void ResetPannel();
};
