// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_SelectShopCoinSlotButton.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UW_SelectShopCoinSlotButton::NativeConstruct()
{
    Super::NativeConstruct();

    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    SelectSlotButton->OnClicked.AddDynamic(this, &UW_SelectShopCoinSlotButton::ClickSlotButton);
    ShopCoinSubsystem->OnCoinCountUpdate.AddDynamic(this, &UW_SelectShopCoinSlotButton::SetSlotCoinCount);
    ShopCoinSubsystem->OnCoinCreated.AddDynamic(this, &UW_SelectShopCoinSlotButton::InitButton);
}

void UW_SelectShopCoinSlotButton::ClickSlotButton()
{

	ShopCoinSubsystem->ChangeCoinSlotByIndex(SlotIndex);
}

void UW_SelectShopCoinSlotButton::SetSlotCoinCount(int32 CurrentSlotIndex, int32 Count)
{
    if(CurrentSlotIndex == SlotIndex)
    {
        SlotCoinCountText->SetText(FText::AsNumber(Count));
    }
}


void UW_SelectShopCoinSlotButton::InitButton(int32 ChangeCoinSlotIndex, EWeaponClass SlotWeaponIndex)
{

    if(ChangeCoinSlotIndex != SlotIndex)
        return;

    if (SelectSlotButton)
    {
        FButtonStyle NewStyle = SelectSlotButton->GetStyle();
        if(SlotWeaponIndex == EWeaponClass::Tank)
            NewStyle.Normal.TintColor = FSlateColor(FColor::FromHex(TEXT("3200FFFF")));
        else if(SlotWeaponIndex == EWeaponClass::Deal)
            NewStyle.Normal.TintColor = FSlateColor(FColor::FromHex(TEXT("FF002DFF")));
        else if(SlotWeaponIndex == EWeaponClass::Heal)
            NewStyle.Normal.TintColor = FSlateColor(FColor::FromHex(TEXT("00FF5BFF")));

        SelectSlotButton->SetStyle(NewStyle);
    }
}