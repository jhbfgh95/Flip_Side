// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SelectWeaponButton.h"
#include "UI/W_CoinCreateWidget.h"
#include "Player/ShopController_FlipSide.h"
#include "Components/Button.h"
#include "Subsystem/CoinCreateWSubsystem.h"

#include "Subsystem/ShopWeaponDataWSubsystem.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Components/Image.h"
#include "Components/TextBlock.h"
void UW_SelectWeaponButton::NativeConstruct()
{
    Super::NativeConstruct();
    //게임 컨트롤러 등에서 참조coinCreateWidget 
    /*
    AShopController_FlipSide* controller = Cast<AShopController_FlipSide>(GetWorld()->GetFirstPlayerController());
    if(controller)
    {
        coinCreateWidget = controller->GetCoinCreateWidget();
        
        
    }*/
    CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();
	WeaponDataSubSystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();

    if(CoinCreateWSubSystem)
    {
        if(WeaponButton)
        {
            WeaponButton->OnClicked.AddDynamic(this, &UW_SelectWeaponButton::SelectWeapon);
        }
    }
}
void UW_SelectWeaponButton::SelectWeapon()
{
    CoinCreateWSubSystem->ChangeSelectedCoinWeapon(WeaponID);
}

void UW_SelectWeaponButton::InitButton(EWeaponClass SettingWeaponClass, int32 Index)
{
    WeaponID = Index;
    WeaponClass =SettingWeaponClass;

    WeaponData = WeaponDataSubSystem->GetWeaponDataByIndex(WeaponClass, WeaponID);
    if(WeaponData)
    {
        
        FButtonStyle ButtonStyle = WeaponButton->GetStyle();
        ButtonStyle.Normal.SetResourceObject(WeaponData->WeaponIcon);
        WeaponButton->SetStyle(ButtonStyle);
        WeaponNameBlock->SetText(FText::FromString(WeaponName));
    }
    else
    {
        /*
        FButtonStyle ButtonStyle = WeaponButton->GetStyle();

        ButtonStyle.Normal.SetResourceObject(WeapoTexture);
        //ButtonStyle.Normal.ImageSize = FVector2D(64.f, 64.f);

        // Pressed
        //ButtonStyle.Pressed.SetResourceObject(WeapoTexture);
        //NewStyle.Pressed.ImageSize = FVector2D(64.f, 64.f);

        WeaponButton->SetStyle(ButtonStyle);

        WeaponNameBlock->SetText(FText::FromString(WeaponName));*/
    }
}