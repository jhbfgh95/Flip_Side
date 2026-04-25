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
            WeaponButton->OnHovered.AddDynamic(this, &UW_SelectWeaponButton::HoverWeapon);
            WeaponButton->OnUnhovered.AddDynamic(this, &UW_SelectWeaponButton::UnHoverWeapon);
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

        ButtonStyle.Normal.SetResourceObject(WeaponData.WeaponIcon);
        ButtonStyle.Hovered.SetResourceObject(WeaponData.WeaponIcon);
        ButtonStyle.Pressed.SetResourceObject(WeaponData.WeaponIcon);
        WeaponButton->SetStyle(ButtonStyle);
        
        WeaponNameBlock->SetText(FText::FromString(WeaponName));

        SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
}
	
void UW_SelectWeaponButton::HoverWeapon()
{
    CoinWSubsystem->HoverWeapon(WeaponData.WeaponID);
}
	
void UW_SelectWeaponButton::UnHoverWeapon()
{
    CoinWSubsystem->UnHoverWeapon();
}
	
UButton* UW_SelectWeaponButton::GetWeaponButton()
{
    return WeaponButton;
}