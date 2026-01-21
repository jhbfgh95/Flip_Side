// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopMainWidget : public UUserWidget
{
	GENERATED_BODY()
	

	protected:
	virtual void NativeConstruct() override;


	class AGameMode_Shop* shopGameMode;
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* GameStartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CoinCreateButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CardSelectButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BuyItemButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CheckBossButton;

public:
	UFUNCTION()
	void ClickCoinCreateButton();

};
