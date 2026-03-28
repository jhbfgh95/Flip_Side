// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopItemDescription.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopItemDescription : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta= (BindWidget))
	class URichTextBlock* ItemNameText;

	UPROPERTY(meta= (BindWidget))
	class URichTextBlock* ItemDesText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UMaterialInstanceDynamic* MID;

public:

	void SetItemInfo(class UTexture2D* Icon,const FString& ItemName, const FString& Description);

	void ResetItemWidget();
};
