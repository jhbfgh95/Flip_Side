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

    NextPageButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponGrid::SetNextPageGrid);
    PreviousPageButton->OnClicked.AddDynamic(this, &UW_UnlockWeaponGrid::SetPreviousPageGrid);

    int32 WeaponCount = UnlockWeaponSubsystem->GetWeaponArrayNum(GridWeaponClass);

    UnlockButtons.Add(UnlockButton1);
    UnlockButtons.Add(UnlockButton2);
    UnlockButtons.Add(UnlockButton3);
    UnlockButtons.Add(UnlockButton4);
    UnlockButtons.Add(UnlockButton5);
    UnlockButtons.Add(UnlockButton6);
    UnlockButtons.Add(UnlockButton7);
    UnlockButtons.Add(UnlockButton8);
    UnlockButtons.Add(UnlockButton9);

    for(int32 i = 0; i < 9; i++)
    {
        int32 WeaponID = UnlockWeaponSubsystem->GetWeaponIDByIndex(GridWeaponClass,i);
        UnlockButtons[i]->InitButton(WeaponID);
    }
    
    SetPageButton();
}

void UW_UnlockWeaponGrid::InitPanelAnimation()
{
    PlayAnimation(OpenPanelAnim, 0.f, 1);
    PauseAnimation(OpenPanelAnim);
    SetAnimationCurrentTime(OpenPanelAnim, 0.f);
}

void UW_UnlockWeaponGrid::SetNextPageGrid()
{
    if(CurrentPage+ 9<= UnlockWeaponSubsystem->GetWeaponArrayNum(GridWeaponClass))
    {
        CurrentPage += 9;
        for(int i =0; i< 9; i++)
        {
            int32 WeaponID = UnlockWeaponSubsystem->GetWeaponIDByIndex(GridWeaponClass,i);
            UnlockButtons[i]->InitButton(WeaponID);
        }
    }
}

void UW_UnlockWeaponGrid::SetPreviousPageGrid()
{
    if(0<= CurrentPage- 9)
    {
        CurrentPage -= 9;
        for(int i =0; i< 9; i++)
        {
            int32 WeaponID = UnlockWeaponSubsystem->GetWeaponIDByIndex(GridWeaponClass,i);
            UnlockButtons[i]->InitButton(WeaponID);
        }
    }
}

void UW_UnlockWeaponGrid::SetPageButton()
{
    if(CurrentPage+ 9<= UnlockSubSystem->GetUnlockWeaponArrrayNum(GridWeaponClass))
    {
        NextPageButton->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        NextPageButton->SetVisibility(ESlateVisibility::Hidden);
    }

    if(0<= CurrentPage- 9)
    {
        PreviousPageButton->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        PreviousPageButton->SetVisibility(ESlateVisibility::Hidden);
    }
}


void UW_UnlockWeaponGrid::OpenUnlockPanel()
{
    PlayAnimation(OpenPanelAnim);
}
    
void UW_UnlockWeaponGrid::CloseUnlockPanel()
{
    PlayAnimation(ClosePanelAnim);
}