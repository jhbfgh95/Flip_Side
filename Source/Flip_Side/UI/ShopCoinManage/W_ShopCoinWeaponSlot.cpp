// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinWeaponSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
void UW_ShopCoinWeaponSlot::NativeOnInitialized()
{   
    Super::NativeOnInitialized();

    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    WeaponSelectButton->OnClicked.AddDynamic(this, &UW_ShopCoinWeaponSlot::ClickWidget);
    WeaponSelectButton->OnHovered.AddDynamic(this, &UW_ShopCoinWeaponSlot::HoverWidget);
    WeaponSelectButton->OnUnhovered.AddDynamic(this, &UW_ShopCoinWeaponSlot::UnhoverWidget);
}

void UW_ShopCoinWeaponSlot::InitWidget(FFaceData FaceData, UShopCoinWSubsystem* InitCoinSubsystem)
{
    WeaponData = FaceData;
    CoinSubsystem = InitCoinSubsystem;
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
    CoinSubsystem->SetWeaponToCoinSide(WeaponData.WeaponID);
}
	
void UW_ShopCoinWeaponSlot::HoverWidget()
{
    CoinSubsystem->HoverWeapon(WeaponData.WeaponID);
}

void UW_ShopCoinWeaponSlot::UnhoverWidget()
{
    CoinSubsystem->UnHoverWeapon();
}