// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/W_ExplainWidget.h"
#include "W_ShopItemExplainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopItemExplainWidget : public UW_ExplainWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;

	UMaterialInstanceDynamic* MID;
public:
	void SetItemImage(UTexture2D* TextureIamge);
	
	void SetItemCount(int32 ItemCount);

	void ResetItemWidget();
};
