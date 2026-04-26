// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopItemInven.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopItemInven : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
private:
	class UShopItemWSubsystem* ShopItemSubsystem;

	class UDataManagerSubsystem* DataManagerSubsystem;


private:
	TArray<class UW_ShopInvenItemButton*> ItemIvens;

	UPROPERTY(meta = (BindWidget))
	class UW_ShopInvenItemButton* InvenItem1;
	UPROPERTY(meta = (BindWidget))
	class UW_ShopInvenItemButton* InvenItem2;
	UPROPERTY(meta = (BindWidget))
	class UW_ShopInvenItemButton* InvenItem3;

private:
	/*
	UPROPERTY(meta = (BindWidget))
	class UButton* InvenHandleButton;
	*/
	UPROPERTY(meta =(BindWidgetAnim), Transient)
	UWidgetAnimation* OpenAnim;
	
	UPROPERTY(meta =(BindWidgetAnim),Transient)
	UWidgetAnimation* CloseAnim;

	bool IsInvenOpen = false;

	UFUNCTION()
	void SetItem(int32 Index);

private:

	UFUNCTION()
	void ActiveInvenHandle();
};
