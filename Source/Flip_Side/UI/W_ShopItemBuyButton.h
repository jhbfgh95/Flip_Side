// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopItemBuyButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopItemBuyButton : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativeConstruct() override;

private:
	class UShopItemWSubsystem* ShopSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UButton* SelectButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* UnSelectButton;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 SelectButtonNum;
public:
	UFUNCTION()
	void SelectItem();
	UFUNCTION()
	void UnSelectItem();
	
	UFUNCTION()
	void HoverItem();

	UFUNCTION()
	void UnHoverItem();
};
