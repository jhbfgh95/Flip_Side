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


    CoinSubsystem->OnHoverWeapon.AddDynamic(this, &UW_ShopSelectCoin::SetWeapon);
    CoinSubsystem->OnSetWeapon.AddDynamic(this, &UW_ShopSelectCoin::SetWeapon);
    CoinSubsystem->OnUnHoverWeapon.AddDynamic(this, &UW_ShopSelectCoin::UnhoverWeapon);
    CoinSubsystem->OnCoinSlotChange.AddDynamic(this, &UW_ShopSelectCoin::SetCoin);

    ChangeCoinSideButton->OnClicked.AddDynamic(this, &UW_ShopSelectCoin::ChangeCoinSide);
    
    
}

void UW_ShopSelectCoin::SetWeapon(int32 WeaponID)
{
    FFaceData SetFaceData;
    DataManager->TryGetWeapon(WeaponID, SetFaceData);

    if(CoinSubsystem->GetIsCreateCoinFront())
    {
        SetFrontWeapon(SetFaceData);
    }
    else
    {
        SetBackWeapon(SetFaceData);
    }
}

void UW_ShopSelectCoin::UnhoverWeapon()
{
    if(CoinSubsystem->GetIsCreateCoinFront())
    {
        SetWeapon(CoinSubsystem->GetCurrentSlotCoin().FrontWeaponID);
    }
    else
    {
        SetWeapon(CoinSubsystem->GetCurrentSlotCoin().BackWeaponID);
    }
}

void UW_ShopSelectCoin::SetFrontWeapon(FFaceData FrontWeaponFaceData)
{
    FrontWeaponData = FrontWeaponFaceData;
    if(FrontWeaponData.WeaponID != -1)
    {
        FrontWeaponImage->SetBrushFromTexture(FrontWeaponData.WeaponIcon);
    }
    else
    {
        FrontWeaponImage->SetBrushFromTexture(DefaultsTexture);
    }

}
void UW_ShopSelectCoin::SetBackWeapon(FFaceData BackWeaponFaceData)
{
    BackWeaponData = BackWeaponFaceData;
    if(BackWeaponData.WeaponID != -1)
    {
        BackWeaponImage->SetBrushFromTexture(BackWeaponData.WeaponIcon);
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
    CoinSubsystem->ChangeCoinSide();
}

void UW_ShopSelectCoin::SetCoin()
{
    FFaceData SetFrontFaceData;
    FFaceData SetBackFaceData;
    
    CurrentCoinType = CoinSubsystem->GetCurrentSlotCoin();

    DataManager->TryGetWeapon(CurrentCoinType.FrontWeaponID,SetFrontFaceData);
    SetFrontWeapon(SetFrontFaceData);

    DataManager->TryGetWeapon(CurrentCoinType.BackWeaponID,SetBackFaceData);
    SetBackWeapon(SetBackFaceData);

}