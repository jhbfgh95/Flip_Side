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

void UW_ShopWeaponSlotContainer::AddWeaponSlot(FFaceData InWeaponData)
{
    UW_ShopCoinWeaponSlot* CoinWeaponSlotWidget =Cast<UW_ShopCoinWeaponSlot>(CreateWidget<UUserWidget>(GetWorld(), WepoanSlotWidget));
    if (CoinWeaponSlotWidget)
    {
        WeaponSlotArray.Add(CoinWeaponSlotWidget);
        UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(CoinWeaponSlotWidget);

        if(VSlot)
            VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

        CoinWeaponSlotWidget->InitWidget(InWeaponData);
    }
}