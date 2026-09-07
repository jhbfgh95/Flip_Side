// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ShopPageTypes.h"
#include "W_ShopCheckStartGame.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopStartGameConfirmed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopStartGameCancelConfirmed, EShopPage, Page);
/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopCheckStartGame : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> StartGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CancelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CheckStartGameText;

public:
	FOnShopStartGameConfirmed OnStartGameConfirmed;
	FOnShopStartGameCancelConfirmed OnShopStartGameCancelConfirmed;
	UFUNCTION(BlueprintCallable)
	void SetCheckStartGameText(bool bHasCoin);

private:
	UFUNCTION()
	void HandleStartGameButtonClicked();
	UFUNCTION()
	void HandleCancelButtonClicked();
};
