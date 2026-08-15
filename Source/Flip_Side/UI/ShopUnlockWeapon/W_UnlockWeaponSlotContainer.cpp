// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlotContainer.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlot.h"

void UW_UnlockWeaponSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();

    DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();


    TArray<FFaceData> LockWeapons = UnlockWeaponSubsystem->GetUnlockWeapons();
    

    for(int i =0; i<LockWeapons.Num(); i++)
    {
        UW_UnlockWeaponSlot* UnlockWeaponSlotWidget =Cast<UW_UnlockWeaponSlot>(CreateWidget<UUserWidget>(GetWorld(), UnlockWeaponSlot));
        if (UnlockWeaponSlotWidget)
        {
            UnlockWeaponSlots.Add(UnlockWeaponSlotWidget);
            UVerticalBoxSlot* VSlot = UnlockWeaponSlotBox->AddChildToVerticalBox(UnlockWeaponSlotWidget);
            if(VSlot)
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            UnlockWeaponSlotWidget->InitWidget(LockWeapons[i], UnlockWeaponSubsystem);
        }
    }

}