// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_CoinSlotWidget.h"

#include "Components/Image.h"
#include "Components/Button.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "WeaponDataTypes.h"
void UW_CoinSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if (FrontWeaponImage)
    {
        FrontDynamicMaterial = FrontWeaponImage->GetDynamicMaterial();
    }
    if(BackWeaponImage)
    {
        BackDynamicMaterial = BackWeaponImage->GetDynamicMaterial();
    }

    CoinSubsystem->OnSetWeapon.AddDynamic(this, &UW_CoinSlotWidget::SetWeaponTexture);
    CoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &UW_CoinSlotWidget::UnlockSlot);
    SlotButton->OnClicked.AddDynamic(this, &UW_CoinSlotWidget::PressSlotButton);
}
	
void UW_CoinSlotWidget::NativeDestruct()
{
    CoinSubsystem->OnUnlockCoinSlot.RemoveAll(this);
    CoinSubsystem->OnSetWeapon.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_CoinSlotWidget::SetWeaponTexture(int32 WeaponID)
{
    
    if(SlotIndex!=CoinSubsystem->GetCurrentSlotNum())
        return;
    FFaceData WeaponData;
    bool IsGetWeaponData = DataSubsystem->TryGetWeapon(WeaponID,WeaponData);

    if(CoinSubsystem->GetIsCreateCoinFront())
    {
        if (FrontDynamicMaterial && FrontDynamicMaterial)
        {
            FrontDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), WeaponData.WeaponIcon);
            FrontDynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), WeaponData.TypeColor);
        }
    }
    else
    {
        if (BackWeaponImage && BackDynamicMaterial)
        {
            BackDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), WeaponData.WeaponIcon);
            BackDynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), WeaponData.TypeColor);
        }
    }
}

	
void UW_CoinSlotWidget::UnlockSlot()
{
    if(SlotIndex==CoinSubsystem->GetCurrentSlotNum())
        LockImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_CoinSlotWidget::PressSlotButton()
{
    CoinSubsystem->SelectCoin(SlotIndex);
}
