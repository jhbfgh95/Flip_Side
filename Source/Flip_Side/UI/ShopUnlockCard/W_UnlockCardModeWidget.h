// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_UnlockCardModeWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockCardModeWidget : public UUserWidget
{
	GENERATED_BODY()


private:
	virtual void NativeConstruct() override;

private:
	class UShopUnlockCardWSubsystem* ShopUnlockCardSubsystem;


private:

private:
	
	UPROPERTY(meta =(BindWidget))
	class UW_UnlockCardWidget* UnlockCardWidget1;
	
	UPROPERTY(meta =(BindWidget))
	class UW_UnlockCardWidget* UnlockCardWidget2;
	
	UPROPERTY(meta =(BindWidget))
	class UW_UnlockCardWidget* UnlockCardWidget3;
	
	UPROPERTY(meta =(BindWidget))
	class UW_UnlockCardWidget* UnlockCardWidget4;

	UPROPERTY(meta = (BindWidget))
	class UButton* LeftPageButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RightPageButton;

private:
	class UUnlockGISubsystem* UnlockSubsystem;

	void InitUnlockCards();
	UFUNCTION()
	void ChangeCardsLeft();
	UFUNCTION()
	void ChangeCardsRight();
};
