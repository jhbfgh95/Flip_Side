// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemDescription.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

#include "Components/Image.h"

void UW_ShopItemDescription::NativeConstruct()
{
    Super::NativeConstruct();
    MID = ItemImage->GetDynamicMaterial();
}
void UW_ShopItemDescription::SetItemInfo(class UTexture2D* Icon,const FString& ItemName, const FString& Description)
{
    FFormatNamedArguments Args;

    if(MID && Icon)
    {
        //MID->SetTextureParameterValue(FName("IconTexture"), Icon);
        //MID->SetScalarParameterValue(FName("IsItemHover"), 1);
    
    }

    FText DescriptionText = FText::FromString(Description);

    ItemDesText->SetText(FText::Format(DescriptionText, Args));

    ItemNameText->SetText(FText::Format(FText::FromString(ItemName),Args));

}


void UW_ShopItemDescription::ResetItemWidget()
{
    if(MID)
        MID->SetScalarParameterValue(FName("IsItemHover"), 0);
    SetItemInfo(nullptr, FString(""),FString(""));
}