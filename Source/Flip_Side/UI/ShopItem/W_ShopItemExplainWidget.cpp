// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemExplainWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UW_ShopItemExplainWidget::SetItemImage(UTexture2D* TextureIamge)
{
    ItemImage->SetOpacity(1);
    ItemImage->SetBrushFromTexture(TextureIamge);
}
	
void UW_ShopItemExplainWidget::SetItemCount(int32 ItemCount)
{
    ItemCountText->SetText(FText::AsNumber(ItemCount));
}

void UW_ShopItemExplainWidget::ResetItemWidget()
{
    ItemImage->SetBrushFromTexture(nullptr);
    ItemImage->SetOpacity(0.f);
    ItemCountText->SetText(FText::GetEmpty());
    SetExplainText(FString(""));
}