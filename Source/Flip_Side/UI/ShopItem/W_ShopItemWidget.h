// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;

private:
	class UShopItemWSubsystem* ShopItemSubsystem;
private:
	UPROPERTY(meta = (BindWidget))
	class UW_ShopItemDescription* ItemDes;


	UFUNCTION()
	void SetDesText(FItemData ItemData);


	UFUNCTION()
	void ShowDescrip();
	
	UFUNCTION()
	void HideDescrip();

};
