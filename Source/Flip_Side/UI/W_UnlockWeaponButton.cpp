// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_UnlockWeaponButton.h"
#include "SubSystem/UnlockGISubsystem.h"

#include "Subsystem/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/ShopWeaponDataWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"

#include "Player/ShopController_FlipSide.h"
#include "Components/Button.h"
#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UW_UnlockWeaponButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
	WeaponDataSubSystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>(); 

    UUnlockGISubsystem* UnlockGISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    if(UnlockGISubsystem)
    {
        UnlockGISubsystem->OnWeaponUnlock.AddDynamic(this, &UW_UnlockWeaponButton::UnlockWeapon);
    }

    WeaponButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponButton::ClickWeaponButton);
}

void UW_UnlockWeaponButton::NativeDestruct()
{
    UUnlockGISubsystem* UnlockGISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    if(UnlockGISubsystem)
    {
        UnlockGISubsystem->OnWeaponUnlock.RemoveAll(this);
    }
}

void UW_UnlockWeaponButton::UnlockWeapon(EWeaponClass SettingWeaponClass, int32 Index)
{
    UE_LOG(LogTemp, Warning, TEXT("무기 해금"));
    if(SettingWeaponClass == WeaponClass && Index == WeaponIndex)
    {
        LockImage->SetVisibility(ESlateVisibility::Collapsed);
    }
}


void UW_UnlockWeaponButton::InitButton(EWeaponClass SettingWeaponClass, int32 Index)
{
    WeaponIndex = Index;
    WeaponClass =SettingWeaponClass;

    WeaponData = WeaponDataSubSystem->GetWeaponDataByIndex(WeaponClass, WeaponIndex);

    if(WeaponData && WeaponButton)
    {
        FButtonStyle ButtonStyle = WeaponButton->GetStyle();

        if (MaskMaterialBase)
        {
            DynamicMaskMaterial = UMaterialInstanceDynamic::Create(MaskMaterialBase, this);

            if (DynamicMaskMaterial)
            {
                DynamicMaskMaterial->SetTextureParameterValue(FName("IconTexture"), WeaponData->WeaponIcon);

                ButtonStyle.Normal.SetResourceObject(DynamicMaskMaterial);
                
                ButtonStyle.Hovered.SetResourceObject(DynamicMaskMaterial);
                ButtonStyle.Pressed.SetResourceObject(DynamicMaskMaterial);
            }
        }
        else
        {
            ButtonStyle.Normal.SetResourceObject(WeaponData->WeaponIcon);
        }

        WeaponButton->SetStyle(ButtonStyle);
        
        WeaponNameBlock->SetText(FText::FromString(WeaponName));

        if(!WeaponDataSubSystem->IsWeaponUnlockByIndex(SettingWeaponClass, WeaponIndex))
        {
            LockImage->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        else
        {
            LockImage->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UW_UnlockWeaponButton::ClickWeaponButton()
{
    UE_LOG(LogTemp, Warning, TEXT("해금 무기선택 누름"));
    UnlockWeaponSubsystem->SelectUnlockWeaponByIndex(WeaponClass, WeaponIndex, WeaponDataSubSystem->IsWeaponUnlockByIndex(WeaponClass, WeaponIndex));
}
