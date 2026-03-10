// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/W_WeaponSelectGrid.h"

#include "UI/ShopCoinCreate/W_SelectWeaponButton.h"
#include "Subsystem/UnlockGISubsystem.h"

#include "Subsystem/ShopLevel/ShopWeaponDataWSubsystem.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"

void UW_WeaponSelectGrid::NativeConstruct()
{
    Super::NativeConstruct();
    
    //this->SetVisibility(ESlateVisibility::Collapsed);
    UnlockSubSystem = GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    ShopWeaponSubsystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();
    UE_LOG(LogTemp, Warning, TEXT("ItemType = %s"),*StaticEnum<EWeaponClass>()->GetNameStringByValue((int64)GridWeaponClass));

    UnlockSubSystem->OnWeaponUnlock.AddDynamic(this, &UW_WeaponSelectGrid::InitWeaponGrid);

    
    int32 UnlockCount = UnlockSubSystem->GetUnlockWeaponArrrayNum(GridWeaponClass);

    int32 CurrentAddNum = 0;

    
    NextPageButton->OnClicked.AddDynamic(this, &UW_WeaponSelectGrid::SetNextPageGrid);
    PreviousPageButton->OnClicked.AddDynamic(this, &UW_WeaponSelectGrid::SetPreviousPageGrid);


    SelectWeaponButtons.Add(SelectWeaponButton1);
    SelectWeaponButtons.Add(SelectWeaponButton2);
    SelectWeaponButtons.Add(SelectWeaponButton3);
    SelectWeaponButtons.Add(SelectWeaponButton4);
    SelectWeaponButtons.Add(SelectWeaponButton5);
    SelectWeaponButtons.Add(SelectWeaponButton6);
    SelectWeaponButtons.Add(SelectWeaponButton7);
    SelectWeaponButtons.Add(SelectWeaponButton8);
    SelectWeaponButtons.Add(SelectWeaponButton9);

    //그리고 무기 해금 상황에 접근해 버튼을 셋팅한다.
    InitWeaponGrid(GridWeaponClass, -1);
    SetPageButton();
}


void UW_WeaponSelectGrid::NativeDestruct()
{
    
    UnlockSubSystem->OnWeaponUnlock.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_WeaponSelectGrid::InitWeaponGrid(EWeaponClass WeaponClass,int32 Index)
{
    if(WeaponClass != GridWeaponClass)
        return;

    //GridButtons
    for(int i =0; i< 9; i++)
    {
        int32 WeaponIndex = UnlockSubSystem->GetUnlockWeaponIndex(GridWeaponClass, i + CurrentPage);
        SelectWeaponButtons[i]->InitButton(GridWeaponClass, WeaponIndex);
    }
}


void UW_WeaponSelectGrid::InitPanelAnimation()
{
    PlayAnimation(OpenWeaponGridAnim, 0.f, 1);
    PauseAnimation(OpenWeaponGridAnim);
    SetAnimationCurrentTime(OpenWeaponGridAnim, 0.f);
}


void UW_WeaponSelectGrid::SetNextPageGrid()
{
    if(CurrentPage+ 9<= UnlockSubSystem->GetUnlockWeaponArrrayNum(GridWeaponClass))
    {
        CurrentPage += 9;
        for(int i =0; i< 9; i++)
        {
            int WeaponIndex = UnlockSubSystem->GetUnlockWeaponIndex(GridWeaponClass,i+CurrentPage);
            SelectWeaponButtons[i]->InitButton(GridWeaponClass, WeaponIndex);
        }
    }
}

void UW_WeaponSelectGrid::SetPreviousPageGrid()
{
    if(0<= CurrentPage- 9)
    {
        CurrentPage -= 9;
        for(int i =0; i< 9; i++)
        {
            int WeaponIndex = UnlockSubSystem->GetUnlockWeaponIndex(GridWeaponClass,i+CurrentPage);
            SelectWeaponButtons[i]->InitButton(GridWeaponClass, WeaponIndex);
        }
    }
}

void UW_WeaponSelectGrid::SetPageButton()
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


void UW_WeaponSelectGrid::OpenWeaponGrid()
{
    PlayAnimation(OpenWeaponGridAnim);
}
void UW_WeaponSelectGrid::CloseWeaponGrid()
{
    PlayAnimation(CloseWeaponGridAnim);
}