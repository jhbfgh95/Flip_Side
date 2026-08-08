// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopWeaponSlotContainer.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopCoinManage/W_ShopCoinWeaponSlot.h"


void UW_ShopWeaponSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    //UnlockSubsystem->OnWeaponUnlock.AddDynamic(this, &UW_ShopWeaponSlotContainer::AddWeaponSlot);

    TArray<int> WeaponID = UnlockSubsystem->GetUnlockWeaponArray(EWeaponClass::Deal);
    
    for(int i =0; i<WeaponID.Num();i++)
    {
        AddWeaponSlot(WeaponID[i]);
    }
}

void UW_ShopWeaponSlotContainer::NativeDestruct()
{
    Super::NativeDestruct();
}
	
void UW_ShopWeaponSlotContainer::AddWeaponSlot(int32 WeaponID)
{
    FFaceData ParamWeaponData;
    DataManager->TryGetWeapon(WeaponID,ParamWeaponData);
    UW_ShopCoinWeaponSlot* CoinWeaponSlotWidget =Cast<UW_ShopCoinWeaponSlot>(CreateWidget<UUserWidget>(GetWorld(), WepoanSlotWidget));
    if (CoinWeaponSlotWidget)
    {
        WeaponSlotArray.Add(CoinWeaponSlotWidget);
        UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(CoinWeaponSlotWidget);

        if(VSlot)
            VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

        CoinWeaponSlotWidget->InitWidget(ParamWeaponData, ShopCoinSubsystem);
    }
}