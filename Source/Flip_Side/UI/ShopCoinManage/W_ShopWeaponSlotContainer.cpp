// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopWeaponSlotContainer.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopCoinManage/W_ShopCoinWeaponSlot.h"


void UW_ShopWeaponSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
}

void UW_ShopWeaponSlotContainer::InitWidget(TArray<FFaceData> InWeaponDataArray)
{
    for(FFaceData InWeaponData : InWeaponDataArray)
    {
        AddWeaponSlot(InWeaponData);
    }
    
}

UW_ShopCoinWeaponSlot* UW_ShopWeaponSlotContainer::AddWeaponSlot(const FFaceData& InWeaponData)
{
    UW_ShopCoinWeaponSlot* CoinWeaponSlotWidget = CreateWidget<UW_ShopCoinWeaponSlot>(GetWorld(), WepoanSlotWidget);
    if (CoinWeaponSlotWidget)
    {
        WeaponSlotArray.Add(CoinWeaponSlotWidget);
        UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(CoinWeaponSlotWidget);

        if(VSlot)
        {
            VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
            VSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
        }

        CoinWeaponSlotWidget->InitWidget(InWeaponData);
        return CoinWeaponSlotWidget;
    }

    return nullptr;
}
