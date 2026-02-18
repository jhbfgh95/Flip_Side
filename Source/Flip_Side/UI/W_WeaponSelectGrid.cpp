// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_WeaponSelectGrid.h"

#include "UI/W_SelectWeaponButton.h"
#include "Subsystem/UnlockGISubsystem.h"

#include "Subsystem/ShopWeaponDataWSubsystem.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
void UW_WeaponSelectGrid::NativeConstruct()
{
    Super::NativeConstruct();
    
    this->SetVisibility(ESlateVisibility::Collapsed);
    UnlockSubSystem = GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    ShopWeaponSubsystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();
    UE_LOG(LogTemp, Warning, TEXT("ItemType = %s"),*StaticEnum<EWeaponClass>()->GetNameStringByValue((int64)GridWeaponClass));

    UnlockSubSystem->OnWeaponUnlock.AddDynamic(this, &UW_WeaponSelectGrid::AddWeaponButton);

    
    int32 UnlockCount = UnlockSubSystem->GetUnlockWeaponArrrayNum(GridWeaponClass);

    CurrentAddNum = 0;

    for(int32 i = 0; i < UnlockCount; i++)
    {
        AddWeaponButton(GridWeaponClass, i);
    }
    
}

void UW_WeaponSelectGrid::AddWeaponButton(EWeaponClass WeaponClass,int32 Index)
{
    if(GridWeaponClass != WeaponClass)
        return;
    UW_SelectWeaponButton* SelectWeaponWidget = CreateWidget<UW_SelectWeaponButton>(GetWorld(), WeaponSelectButton);
    if (SelectWeaponWidget)
    {
        int32 Row = CurrentAddNum / ColumnCount;
        int32 Col = CurrentAddNum % ColumnCount;

        UUniformGridSlot* SlotWidget = weaponButtonGrid->AddChildToUniformGrid(SelectWeaponWidget, Row, Col);

        SelectWeaponWidget->InitButton(GridWeaponClass, Index);


        if (SlotWidget)
        {
            //SlotWidget->SetHorizontalAlignment(HAlign_Fill);
            //SlotWidget->SetVerticalAlignment(VAlign_Fill);
        }

        CurrentAddNum++;

    }
}