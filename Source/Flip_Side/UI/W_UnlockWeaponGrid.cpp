// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_UnlockWeaponGrid.h"
#include "SubSystems/WorldSubsystem.h"
#include "SubSystem/ShopWeaponDataWSubsystem.h"
#include "SubSystem/UnlockGISubSystem.h"
#include "UI/W_UnlockWeaponButton.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
void UW_UnlockWeaponGrid::NativeConstruct()
{
    Super::NativeConstruct();
    WeaponDataSubSystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();
    UnlockSubSystem= GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();



    int32 WeaponCount = WeaponDataSubSystem->GetWeaponArrayNum(GridWeaponClass);

    for(int32 i = 0; i < WeaponCount; i++)
    {
        UW_UnlockWeaponButton* UnlockButtonClass = CreateWidget<UW_UnlockWeaponButton>(GetWorld(), WeaponUnlockButton);

        if (UnlockButtonClass)
        {
           
            int32 Row = i / ColumnCount;
            int32 Col = i % ColumnCount;

            UUniformGridSlot* SlotWidget = UnlockGrid->AddChildToUniformGrid(UnlockButtonClass, Row, Col);

            UnlockButtonClass->InitButton(GridWeaponClass,i);
            //UnlockSubSystem->IsWeaponUnlockByIndex(GridWeaponClass, i)
            if (SlotWidget)
            {
                SlotWidget->SetHorizontalAlignment(HAlign_Fill);
                SlotWidget->SetVerticalAlignment(VAlign_Fill);
            }
        }
    }
}