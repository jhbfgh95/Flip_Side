// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCoinManage/GoToCreateCoinPanel.h"
#include "Tuto_GoToCreateCoinPanel.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATuto_GoToCreateCoinPanel : public AGoToCreateCoinPanel
{
	GENERATED_BODY()
	

	protected:
	virtual void BeginPlay() override;

	class UShopTutorialWSubsystem* TutorialSubsystem;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	int32 ExecuteOrder;
	
	virtual void InteractLeftClick_Implementation() override;
};
