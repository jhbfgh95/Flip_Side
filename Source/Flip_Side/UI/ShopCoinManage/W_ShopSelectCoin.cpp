// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopSelectCoin.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/Button.h"
#include "Components/Image.h"
void UW_ShopSelectCoin::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    ChangeCoinSideButton->OnClicked.AddDynamic(this, &UW_ShopSelectCoin::ChangeCoinSide);
}


void UW_ShopSelectCoin::SetFrontWeapon(FFaceData FrontWeaponFaceData)
{
    if(FrontWeaponFaceData.WeaponID != -1)
    {
        FrontWeaponImage->SetBrushFromTexture(FrontWeaponFaceData.WeaponIcon);
    }
    else
    {
        FrontWeaponImage->SetBrushFromTexture(DefaultsTexture);
    }

}
void UW_ShopSelectCoin::SetBackWeapon(FFaceData BackWeaponFaceData)
{
    if(BackWeaponFaceData.WeaponID != -1)
    {
        BackWeaponImage->SetBrushFromTexture(BackWeaponFaceData.WeaponIcon);
    }
    else
    { 
        BackWeaponImage->SetBrushFromTexture(DefaultsTexture);
    }
}

void UW_ShopSelectCoin::ResetCoin()
{
    FrontWeaponImage->SetBrushFromTexture(DefaultsTexture);
    BackWeaponImage->SetBrushFromTexture(DefaultsTexture);
}

void UW_ShopSelectCoin::ChangeCoinSide()
{
    OnChangeShopSelectedCoinSide.Broadcast();
}