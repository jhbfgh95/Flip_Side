// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemExplainWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UW_ShopItemExplainWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    MID = ItemImage->GetDynamicMaterial();
}
void UW_ShopItemExplainWidget::SetItemImage(UTexture2D* TextureIamge)
{
    if(MID)
    {
        MID->SetTextureParameterValue(FName("IconTexture"), TextureIamge);
        MID->SetScalarParameterValue(FName("IsItemHover"), 1);
    }
    
}
	
void UW_ShopItemExplainWidget::SetItemCount(int32 ItemCount)
{
    ItemCountText->SetText(FText::AsNumber(ItemCount));
}

void UW_ShopItemExplainWidget::ResetItemWidget()
{
    if(MID)
        MID->SetScalarParameterValue(FName("IsItemHover"), 0);
    //ItemImage->SetBrushFromTexture(nullptr);
    //ItemImage->SetOpacity(0.f);
    ItemCountText->SetText(FText::GetEmpty());
    SetExplainText(FString(""));
}