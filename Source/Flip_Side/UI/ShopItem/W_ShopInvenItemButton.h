// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/ItemDataTypes.h"
#include "W_ShopInvenItemButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopInvenItemButton : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	class UShopItemWSubsystem* ItemSubsystem;
	class UDataManagerSubsystem* DataSubsystem;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ItemButton;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;


private:
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	int32 ItemSlotNum;

	int32 ItemCount;

	FItemData CurrentItemData;

private:
	UFUNCTION()
	void HoverItem();

public:
	void SetItemData(FItemData ItemData);

	void SetCountText(int32 Count);
	
	void SetItemImage(UTexture2D* Texture);


};
