// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/W_CardWidget.h"
#include "W_UnlockCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockCardWidget : public UW_CardWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	class UShopCardWSubsystem* CardSubsystem;
	class UUnlockGISubsystem* UnlockSubsystem;

private:
	UPROPERTY(meta = (BindWidet))
	class UImage* LockImage;


private:
	void InitUnlockCard(FCardData CardData);

};
