// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/ShopTutorialInterface.h"
#include "W_Tuto_ExplainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_ExplainWidget : public UUserWidget, public IShopTutorialInterface
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

	virtual void ExecuteTutorialEvent_Implementation() override;
	
	virtual void FinishTutorialEvent_Implementation() override;
private:
	class UShopTutorialWSubsystem* TutorialSubsystem;

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MessageText;

private:
	int32 CurrentIndex = 0;
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess), meta = (MultiLine = true))
	TArray<FString> Messages;
	
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	int EvenOrder = 0;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	int NextOrder = 0;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	bool IsExecuteNextOrder = true;

	void OnClickNext();
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
};
