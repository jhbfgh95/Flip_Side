// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopUnlockCoinSlot.h"
#include "Components/Button.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

void UW_ShopUnlockCoinSlot::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    ShopCoinSubsystem->OnCoinSlotChange.AddDynamic(this, &UW_ShopUnlockCoinSlot::SetWidgetActive);
    ShopCoinSubsystem->OnUnlockCoinSlot.AddDynamic(this, &UW_ShopUnlockCoinSlot::SetWidgetActive);
    UnlockCoinSlotButton->OnClicked.AddDynamic(this, &UW_ShopUnlockCoinSlot::ClickUnlockButton);
    SetWidgetActive();
}

void UW_ShopUnlockCoinSlot::NativeDestruct()
{
    ShopCoinSubsystem->OnCoinSlotChange.RemoveAll(this);
    ShopCoinSubsystem->OnUnlockCoinSlot.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_ShopUnlockCoinSlot::SetWidgetActive()
{
    if(ShopCoinSubsystem->GetCurrentCoinUnlock())
    {
        UnlockCoinSlotButton->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        UnlockCoinSlotButton->SetVisibility(ESlateVisibility::Visible);
    }
}
void UW_ShopUnlockCoinSlot::ClickUnlockButton()
{
    ShopCoinSubsystem->UnlockCurrentCoinSlot();
}
