// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinWeaponSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"

void UW_ShopCoinWeaponSlot::NativeOnInitialized()
{   
    Super::NativeOnInitialized();

    WeaponSelectButton->OnClicked.AddDynamic(this, &UW_ShopCoinWeaponSlot::ClickWidget);
    WeaponSelectButton->OnHovered.AddDynamic(this, &UW_ShopCoinWeaponSlot::HoverWidget);
    WeaponSelectButton->OnUnhovered.AddDynamic(this, &UW_ShopCoinWeaponSlot::UnhoverWidget);

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }
    if (SelectBorder)
    {
        SelectBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UW_ShopCoinWeaponSlot::InitWidget(FFaceData FaceData)
{
    WeaponData = FaceData;
    if(FaceData.WeaponID != -1)
    {
        WeaponIcon->SetBrushFromTexture(FaceData.WeaponIcon);

        WeaponName->SetText(FText::FromString(FaceData.WeaponName));

        SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
}
	
void UW_ShopCoinWeaponSlot::ClickWidget()
{
    OnClickedShopCoinWeaponSlot.Broadcast(this);
}
	
void UW_ShopCoinWeaponSlot::HoverWidget()
{
    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    OnHoveredShopCoinWeaponSlot.Broadcast(WeaponData.WeaponID);
}

void UW_ShopCoinWeaponSlot::UnhoverWidget()
{
    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }

    OnUnhoveredShopCoinWeaponSlot.Broadcast();
}


int32 UW_ShopCoinWeaponSlot::GetWeaponSlotID()
{
    return WeaponData.WeaponID;
}
	
void UW_ShopCoinWeaponSlot::SelectSlot()
{
    if(IsSelected)
        return;
    SelectBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    IsSelected = true;
}
	
void UW_ShopCoinWeaponSlot::UnSelectSlot()
{
    if(!IsSelected)
        return;
    SelectBorder->SetVisibility(ESlateVisibility::Hidden);
    IsSelected = false;
}
