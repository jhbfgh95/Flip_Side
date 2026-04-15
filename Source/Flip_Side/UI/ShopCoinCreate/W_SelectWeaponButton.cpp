// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/W_SelectWeaponButton.h"
#include "UI/ShopCoinCreate/W_CoinCreateWidget.h"
#include "Player/ShopController_FlipSide.h"
#include "Components/Button.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"

#include "Subsystems/WorldSubsystem.h" 
#include "Components/Image.h"
#include "Components/TextBlock.h"
void UW_SelectWeaponButton::NativeConstruct()
{
    Super::NativeConstruct();

    CoinWSubsystem =  GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    DataManager = GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    if(CoinWSubsystem)
    {
        if(WeaponButton)
        {
            WeaponButton->OnClicked.AddDynamic(this, &UW_SelectWeaponButton::SelectWeapon);
        }
    }
}

void UW_SelectWeaponButton::SelectWeapon()
{
    CoinWSubsystem->SetWeaponToCoinSide(WeaponData.WeaponID);
}

void UW_SelectWeaponButton::InitButton(int32 ID)
{

    bool IsGetWeaponData = DataManager->TryGetWeapon(ID,WeaponData);

    if(IsGetWeaponData && WeaponButton)
    {
        FButtonStyle ButtonStyle = WeaponButton->GetStyle();

        if (MaskMaterialBase)
        {
            DynamicMaskMaterial = UMaterialInstanceDynamic::Create(MaskMaterialBase, this);

            if (DynamicMaskMaterial)
            {
                DynamicMaskMaterial->SetTextureParameterValue(FName("IconTexture"), WeaponData.WeaponIcon);

                ButtonStyle.Normal.SetResourceObject(DynamicMaskMaterial);
                
                ButtonStyle.Hovered.SetResourceObject(DynamicMaskMaterial);
                ButtonStyle.Pressed.SetResourceObject(DynamicMaskMaterial);
            }
        }
        else
        {
            ButtonStyle.Normal.SetResourceObject(WeaponData.WeaponIcon);
        }

        WeaponButton->SetStyle(ButtonStyle);
        
        WeaponNameBlock->SetText(FText::FromString(WeaponName));

        SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
}