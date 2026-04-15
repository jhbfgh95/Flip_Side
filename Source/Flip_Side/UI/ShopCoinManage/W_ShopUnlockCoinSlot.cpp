// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopUnlockCoinSlot.h"
#include "Components/Button.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

void UW_ShopUnlockCoinSlot::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &UW_ShopUnlockCoinSlot::SetWidgetActive);

    UnlockCoinSlotButton->OnClicked.AddDynamic(this, &UW_ShopUnlockCoinSlot::ClickUnlockButton);
    SetWidgetActive(true);
}

void UW_ShopUnlockCoinSlot::NativeDestruct()
{
    ShopCoinSubsystem->OnCoinSlotChange.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_ShopUnlockCoinSlot::SetWidgetActive(bool IsMoveBottom)
{
    if(ShopCoinSubsystem->GetCurrentCoinUnlock())
    {
        SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        SetVisibility(ESlateVisibility::Visible);
    }
}
void UW_ShopUnlockCoinSlot::ClickUnlockButton()
{
    ShopCoinSubsystem->UnlockCurrentCoinSlot();
}
