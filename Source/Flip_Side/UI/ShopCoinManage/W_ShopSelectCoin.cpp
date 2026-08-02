// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopSelectCoin.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/Button.h"
#include "Components/Image.h"
void UW_ShopSelectCoin::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    if (FrontWeaponImage)
    {
        FrontDynamicMaterial = FrontWeaponImage->GetDynamicMaterial();
    }
    if(BackWeaponImage)
    {
        BackDynamicMaterial = BackWeaponImage->GetDynamicMaterial();
    }
}

void UW_ShopSelectCoin::SetFrontWeapon(FFaceData FrontWeaponFaceData)
{
    FrontWeaponData = FrontWeaponFaceData;
    if(FrontWeaponData.WeaponID != -1)
    {
        FrontDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), FrontWeaponData.WeaponIcon);
        FrontDynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), FrontWeaponData.TypeColor);
    }
    else
    {
        FrontDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), DefaultsTexture);
    }

}
void UW_ShopSelectCoin::SetBackWeapon(FFaceData BackWeaponFaceData)
{
    BackWeaponData = BackWeaponFaceData;
    if(BackWeaponData.WeaponID != -1)
    {
        BackDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), BackWeaponData.WeaponIcon);
        BackDynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), BackWeaponData.TypeColor);
    }
    else
    {
        BackDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), DefaultsTexture);
    }
}

void UW_ShopSelectCoin::ChangeCoinSide()
{

}

void UW_ShopSelectCoin::SetCoin()
{
    FFaceData SetFaceData;
    CurrentCoinType = CoinSubsystem->GetCurrentSlotCoin();
    DataManager->TryGetWeapon(CurrentCoinType.FrontWeaponID,SetFaceData);
    SetFrontWeapon(SetFaceData);

    DataManager->TryGetWeapon(CurrentCoinType.BackWeaponID,SetFaceData);
    SetBackWeapon(SetFaceData);

}