// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopSelectedItem.h"

#include "Components/Image.h"

void UW_ShopSelectedItem::SetImage(UTexture2D* InItemTexture)
{
	if (IsValid(ItemImage))
	{
		ItemImage->SetBrushFromTexture(InItemTexture);
	}
}

