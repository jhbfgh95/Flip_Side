// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopUnlockCard/W_UnlockCardModeWidget.h"
#include "W_Tuto_UnlockCardModeWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_UnlockCardModeWidget : public UW_UnlockCardModeWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	
	class UShopTutorialWSubsystem* TutoSubsystem;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 UnlockExcuteOrder;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 SelectExcuteOrder;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 UnselectExcuteOrder;

	UFUNCTION()
	void UnlockExcuteEvent();

	UFUNCTION()
	void SelectExcuteEvent();

	UFUNCTION()
	void UnSelectExcuteEvent();


};
