// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponGrid.h"
#include "SubSystems/WorldSubsystem.h"
#include "SubSystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "SubSystem/UnlockGISubSystem.h"

#include "UI/ShopUnlockWeapon/W_UnlockWeaponButton.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"
void UW_UnlockWeaponGrid::NativeConstruct()
{
    Super::NativeConstruct();

    UnlockSubSystem= GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();

    int32 WeaponCount = UnlockWeaponSubsystem->GetWeaponArrayNum(GridWeaponClass);

    InitWeaponGrid();

}

void UW_UnlockWeaponGrid::NativeDestruct()
{
    Super::NativeDestruct();
}

void UW_UnlockWeaponGrid::InitWeaponGrid()
{
    for(int i =0; i< UnlockWeaponSubsystem->GetWeaponArrayNum(GridWeaponClass); i++)
    {
        int32 WeaponID = UnlockWeaponSubsystem->GetWeaponIDByIndex(GridWeaponClass, i);

        AddButton(WeaponID);
    }
}

void UW_UnlockWeaponGrid::AddButton(int32 ID)
{
    if(ColumnCount==0)
        return;
    if(!WeaponUnlockButton)
        return;
    int32 Row = UnlockButtons.Num() / ColumnCount;
    int32 Col = UnlockButtons.Num() % ColumnCount;

    UW_UnlockWeaponButton* WeaponButton = CreateWidget<UW_UnlockWeaponButton>(GetWorld(), WeaponUnlockButton);
    
    if(WeaponButton)
    {
        ButtonGrid->AddChildToUniformGrid(WeaponButton, Row, Col);
        WeaponButton->InitButton(ID);
        UnlockButtons.Add(WeaponButton);
    }
}
