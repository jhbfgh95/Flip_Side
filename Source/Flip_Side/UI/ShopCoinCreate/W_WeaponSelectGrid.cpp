// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinCreate/W_WeaponSelectGrid.h"

#include "UI/ShopCoinCreate/W_SelectWeaponButton.h"
#include "Subsystem/UnlockGISubsystem.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UW_WeaponSelectGrid::NativeConstruct()
{
    Super::NativeConstruct();
    
    //this->SetVisibility(ESlateVisibility::Collapsed);
    UnlockSubSystem = GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    UnlockSubSystem->OnWeaponUnlock.AddDynamic(this, &UW_WeaponSelectGrid::OnWeaponUnlockAdaptor);

    
    int32 UnlockCount = UnlockSubSystem->GetUnlockWeaponArrrayNum(GridWeaponClass);

    int32 CurrentAddNum = 0;

    //그리고 무기 해금 상황에 접근해 버튼을 셋팅한다.
    InitWeaponGrid();
    HideDescripText();
}

void UW_WeaponSelectGrid::NativeDestruct()
{
    UnlockSubSystem->OnWeaponUnlock.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_WeaponSelectGrid::InitWeaponGrid()
{
    //GridButtons
    for(int i =0; i< UnlockSubSystem->GetUnlockWeaponArrrayNum(GridWeaponClass); i++)
    {
        int32 WeaponID = UnlockSubSystem->GetUnlockWeaponID(GridWeaponClass, i);

        AddButton(WeaponID);
    }
}


void UW_WeaponSelectGrid::InitPanelAnimation()
{
    PlayAnimation(OpenWeaponGridAnim, 0.f, 1);
    PauseAnimation(OpenWeaponGridAnim);
    SetAnimationCurrentTime(OpenWeaponGridAnim, 0.f);
}

void UW_WeaponSelectGrid::OpenWeaponGrid()
{
    PlayAnimation(OpenWeaponGridAnim);
}
void UW_WeaponSelectGrid::CloseWeaponGrid()
{
    PlayAnimation(CloseWeaponGridAnim);
}

void UW_WeaponSelectGrid::AddButton(int32 ID)
{
    if(ColumnCount==0)
        return;
    if(!WeaponSelectButton)
        return;
    int32 Row = SelectWeaponButtons.Num() / ColumnCount;
    int32 Col = SelectWeaponButtons.Num() % ColumnCount;

    UW_SelectWeaponButton* WeaponButton = CreateWidget<UW_SelectWeaponButton>(GetWorld(), WeaponSelectButton);
    
    if(WeaponButton)
    {
        ButtonGrid->AddChildToUniformGrid(WeaponButton, Row, Col);
        WeaponButton->InitButton(ID);
        WeaponButton->GetWeaponButton()->OnHovered.AddDynamic(this, &UW_WeaponSelectGrid::ShowDescripText);
        WeaponButton->GetWeaponButton()->OnUnhovered.AddDynamic(this,&UW_WeaponSelectGrid::HideDescripText);
        SelectWeaponButtons.Add(WeaponButton);
    }
}


void UW_WeaponSelectGrid::OnWeaponUnlockAdaptor(EWeaponClass WeaponClass,int32 ID)
{
    if(WeaponClass != GridWeaponClass)
        return;

    int32 LastWeaponID = UnlockSubSystem->GetUnlockWeaponID(GridWeaponClass,UnlockSubSystem->GetUnlockWeaponArrrayNum(GridWeaponClass) -1);
    AddButton(LastWeaponID);
}


void UW_WeaponSelectGrid::ShowDescripText()
{
    DescripText->SetVisibility(ESlateVisibility::Visible);
}

void UW_WeaponSelectGrid::HideDescripText()
{
    DescripText->SetVisibility(ESlateVisibility::Hidden);
}