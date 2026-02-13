// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_UnlockWeaponButton.h"
#include "SubSystem/UnlockGISubsystem.h"
#include "SubSystem/ShopWeaponDataWSubsystem.h"
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
    
	UnlockSubSystem = GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    WeaponDataSubSystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();


    UnlockButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponButton::UnlockWeapon);
}


void UW_UnlockWeaponButton::UnlockWeapon()
{
    UnlockSubSystem->UnlockWeapon(WeaponClass, Index);
}


void UW_UnlockWeaponButton::InitButton(EWeaponClass SetWeaponClass, int32 IndexValue)
{
    WeaponClass = SetWeaponClass;
    Index = IndexValue;
    
    WeaponData = WeaponDataSubSystem->GetWeaponDataByIndex(WeaponClass, Index);
    
    if(WeaponData)
    {
        FButtonStyle ButtonStyle = UnlockButton->GetStyle();
        ButtonStyle.Normal.SetResourceObject(WeaponData->WeaponIcon);
        UnlockButton->SetStyle(ButtonStyle);
        //WeaponNameBlock->SetText(FText::FromString(WeaponName));
    }
}

