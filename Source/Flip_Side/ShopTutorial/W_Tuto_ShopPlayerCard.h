// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCard/W_ShopPlayerCard.h"
#include "W_Tuto_ShopPlayerCard.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_ShopPlayerCard : public UW_ShopPlayerCard
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	
	class UShopTutorialWSubsystem* TutoSubsystem;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 ExcuteOrder;
protected:
	virtual void RClickCard() override;
};
