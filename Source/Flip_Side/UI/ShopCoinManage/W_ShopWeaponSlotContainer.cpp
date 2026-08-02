// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopWeaponSlotContainer.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopCoinManage/W_CoinWeaponSlot.h"


void UW_ShopWeaponSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    //TArray<int> WeaponFaceData = UnlockSubsystem->GetUnlockWeaponArray(EWeaponClass::Deal);

    for(int i =0; i<3;i++)
    {
        UW_CoinWeaponSlot* CoinWeaponSlotWidget =Cast<UW_CoinWeaponSlot>(CreateWidget<UUserWidget>(GetWorld(), WepoanSlotWidget));
        if (CoinWeaponSlotWidget)
        {
            WeaponSlotArray.Add(CoinWeaponSlotWidget);
            UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(CoinWeaponSlotWidget);

            if(VSlot)
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

            //CoinWeaponSlotWidget->InitWidget(i);
        }
    }
}


void UW_ShopWeaponSlotContainer::NativeDestruct()
{
    Super::NativeDestruct();
}