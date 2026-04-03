// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopCard/W_CardSelectWidget.h"
#include "W_Tuto_CardSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_CardSelectWidget : public UW_CardSelectWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
private:
	class UShopTutorialWSubsystem* TutorialSubsytem;

private:
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 NextOrder = -1;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsExecuteNextOrder = true;
	
private:
	UFUNCTION()
	void ExecuteNextTutorial();
	
};
