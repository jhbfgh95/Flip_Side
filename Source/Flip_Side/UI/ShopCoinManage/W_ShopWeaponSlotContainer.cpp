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

	
void UW_ShopWeaponSlotContainer::SelectWeaponSlots(int32 FrontWeaponID, int32 BackWeaponID)
{
    UnSelectWeaponSlots();

    for(UW_ShopCoinWeaponSlot* WeaponSlot : WeaponSlotArray)
    {
        if (!IsValid(WeaponSlot))
        {
            continue;
        }

        if(WeaponSlot->GetWeaponSlotID() == FrontWeaponID)
        {
            SelectFrontWeaponSlot = WeaponSlot;
            WeaponSlot->SelectSlot();
        }
        if(WeaponSlot->GetWeaponSlotID() == BackWeaponID)
        {
            SelectBackWeaponSlot = WeaponSlot;
            WeaponSlot->SelectSlot();
        }
    }
}

void UW_ShopWeaponSlotContainer::SelectWeaponSlot(
    bool bIsFrontWeapon, UW_ShopCoinWeaponSlot* ClickedWeaponSlot)
{
    if (!IsValid(ClickedWeaponSlot))
    {
        return;
    }

    TObjectPtr<UW_ShopCoinWeaponSlot>& CurrentWeaponSlot =
        bIsFrontWeapon ? SelectFrontWeaponSlot : SelectBackWeaponSlot;
    TObjectPtr<UW_ShopCoinWeaponSlot>& OtherWeaponSlot =
        bIsFrontWeapon ? SelectBackWeaponSlot : SelectFrontWeaponSlot;

    if (IsValid(CurrentWeaponSlot) && CurrentWeaponSlot != ClickedWeaponSlot &&
        CurrentWeaponSlot != OtherWeaponSlot)
    {
        CurrentWeaponSlot->UnSelectSlot();
    }

    CurrentWeaponSlot = ClickedWeaponSlot;
    ClickedWeaponSlot->SelectSlot();
}

void UW_ShopWeaponSlotContainer::UnSelectWeaponSlots()
{
    if (IsValid(SelectFrontWeaponSlot))
    {
        SelectFrontWeaponSlot->UnSelectSlot();
    }
    if (IsValid(SelectBackWeaponSlot) && SelectBackWeaponSlot != SelectFrontWeaponSlot)
    {
        SelectBackWeaponSlot->UnSelectSlot();
    }

    SelectFrontWeaponSlot = nullptr;
    SelectBackWeaponSlot = nullptr;
}
