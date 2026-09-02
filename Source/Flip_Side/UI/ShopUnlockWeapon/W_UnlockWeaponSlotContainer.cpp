// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlotContainer.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlot.h"

void UW_UnlockWeaponSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
}

void UW_UnlockWeaponSlotContainer::InitWidget(const TArray<FFaceData>& InWeaponDataArray)
{
    UnlockWeaponSlots.Empty();
    UnlockWeaponSlotBox->ClearChildren();

    for(const FFaceData& WeaponData : InWeaponDataArray)
    {
        AddWeaponSlot(WeaponData);
    }
}

void UW_UnlockWeaponSlotContainer::AddWeaponSlot(const FFaceData& InWeaponData)
{
    UW_UnlockWeaponSlot* SlotWidget = CreateWidget<UW_UnlockWeaponSlot>(GetWorld(), UnlockWeaponSlot);
    if(!IsValid(SlotWidget))
        return;

    UnlockWeaponSlots.Add(SlotWidget);
    UVerticalBoxSlot* VerticalSlot = UnlockWeaponSlotBox->AddChildToVerticalBox(SlotWidget);
    if(VerticalSlot)
        VerticalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

    SlotWidget->InitWidget(InWeaponData);
}

bool UW_UnlockWeaponSlotContainer::RemoveWeaponSlot(int32 WeaponID)
{
    for(int32 Index = 0; Index < UnlockWeaponSlots.Num(); ++Index)
    {
        UW_UnlockWeaponSlot* SlotWidget = UnlockWeaponSlots[Index];
        if(IsValid(SlotWidget) && SlotWidget->GetWeaponID() == WeaponID)
        {
            UnlockWeaponSlotBox->RemoveChild(SlotWidget);
            UnlockWeaponSlots.RemoveAt(Index);
            return true;
        }
    }

    return false;
}
