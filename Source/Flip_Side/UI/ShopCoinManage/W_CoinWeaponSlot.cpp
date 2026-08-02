// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_CoinWeaponSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
void UW_CoinWeaponSlot::NativeOnInitialized()
{   
    Super::NativeOnInitialized();

    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    WeaponSelectButton->OnClicked.AddDynamic(this, &UW_CoinWeaponSlot::ClickWidget);
}

void UW_CoinWeaponSlot::InitWidget(FFaceData FaceData)
{

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
	
void UW_CoinWeaponSlot::ClickWidget()
{
    //CoinSubsystem->SetWeaponToCoinSide();
}
	
void UW_CoinWeaponSlot::HoverWidget()
{
}

void UW_CoinWeaponSlot::UnhoverWidget()
{

}