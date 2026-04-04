// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/ShopTutorialInterface.h"
#include "W_ShopTuto_Blocker.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopTuto_Blocker : public UUserWidget, public IShopTutorialInterface
{
	GENERATED_BODY()
	private:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TArray<int32> EventOrder;
private:
	class UShopTutorialWSubsystem* TutorialSubsystem;
private:

	virtual void ExecuteTutorialEvent_Implementation() override;
	virtual void FinishTutorialEvent_Implementation() override;
};
