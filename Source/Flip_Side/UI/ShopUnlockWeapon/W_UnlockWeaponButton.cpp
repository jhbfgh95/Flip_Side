// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponButton.h"
#include "SubSystem/UnlockGISubsystem.h"

#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"

#include "Player/ShopController_FlipSide.h"
#include "Components/Button.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UW_UnlockWeaponButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>(); 
    UnlockGISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    if(UnlockGISubsystem)
    {
        UnlockGISubsystem->OnWeaponUnlock.AddDynamic(this, &UW_UnlockWeaponButton::UnlockWeapon);
    }

    WeaponButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponButton::ClickWeaponButton);
    PlayAnimation(FadeAnim,0,0);
}

void UW_UnlockWeaponButton::NativeDestruct()
{
    UnlockGISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    if(UnlockGISubsystem)
    {
        UnlockGISubsystem->OnWeaponUnlock.RemoveAll(this);
    }
    Super::NativeDestruct();
}

void UW_UnlockWeaponButton::UnlockWeapon(EWeaponClass SettingWeaponClass, int32 ID)
{
    if(SettingWeaponClass == WeaponData.WeaponType && WeaponData.WeaponID == ID)
    {
        LockImage->SetVisibility(ESlateVisibility::Collapsed);
    }
}


void UW_UnlockWeaponButton::InitButton(int32 ID)
{
    bool IsGetWeaponData = DataManager->TryGetWeapon(ID, WeaponData);

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

        if(UnlockGISubsystem->IsWeaponUnlockByID(WeaponData.WeaponType, WeaponData.WeaponID))
        {
            LockImage->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            LockImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void UW_UnlockWeaponButton::ClickWeaponButton()
{
    UnlockWeaponSubsystem->SelectUnlockWeapon(WeaponData.WeaponType, WeaponData.WeaponID, 
    UnlockGISubsystem->IsWeaponUnlockByID(WeaponData.WeaponType, WeaponData.WeaponID));
}
