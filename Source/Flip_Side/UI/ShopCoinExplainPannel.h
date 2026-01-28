// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ExplainPannel.h"
#include "ShopCoinExplainPannel.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AShopCoinExplainPannel : public AExplainPannel
{
	GENERATED_BODY()
	
private:
	

	class UCoinCreateWSubsystem* CoinCreateWSubSystem;

protected:
	virtual void BeginPlay() override;

public:

	virtual void GetExplainByID(int32 ID) override;

};
