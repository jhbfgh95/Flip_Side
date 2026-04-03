// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShopUnlockCard/W_UnlockCardWidget.h"
#include "W_Tuto_UnlockCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_UnlockCardWidget : public UW_UnlockCardWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
private:
	class UShopTutorialWSubsystem* TutorialSubsytem;

private:
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 NextOrder = -1;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsExecuteNextOrder = true;
	
private:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION()
	void ExecuteNextTutorial();
};
