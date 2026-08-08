// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlot.h"

#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "WeaponDataTypes.h"
#include "Components/Border.h"

void UW_ShopCoinSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    CoinSubsystem->OnSetWeapon.AddDynamic(this, &UW_ShopCoinSlot::SetWeaponTexture);

    CoinSubsystem->OnCoinCountUpdate.AddDynamic(this, &UW_ShopCoinSlot::SetCountText);
    CoinSubsystem->OnCoinSlotChange.AddDynamic(this, &UW_ShopCoinSlot::SetBackGround);

    SlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::PressSlotButton);


    BackGroundBorder->SetRenderOpacity(0.7f);
    
}
	
void UW_ShopCoinSlot::NativeDestruct()
{
    CoinSubsystem->OnSetWeapon.RemoveAll(this);
    CoinSubsystem->OnCoinSlotChange.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_ShopCoinSlot::SetWeaponTexture(int32 WeaponID)
{
    if(SlotIndex !=CoinSubsystem->GetCurrentSlotNum())
        return;

    if(CoinSubsystem->GetIsCreateCoinFront())
        SetFrontWeaponImage(WeaponID);
    else
        SetBackWeaponImage(WeaponID);

}


void UW_ShopCoinSlot::PressSlotButton()
{
    CoinSubsystem->SelectCoin(SlotIndex);
}

void UW_ShopCoinSlot::SetCountText(int32 SlotNum, int32 Count)
{
    if(SlotNum != SlotIndex)
        return;

    CoinCountText->SetText(FText::AsNumber(Count));
}

	
void UW_ShopCoinSlot::SetBackGround()
{
    /*
    if(CoinSubsystem->GetCurrentSlotNum()==SlotIndex)
        BackGroundBorder->SetRenderOpacity(1.2f);
    else
        BackGroundBorder->SetRenderOpacity(0.7f);*/
}
	
void UW_ShopCoinSlot::InitSlot(int32 SlotNum)
{
    SlotIndex = SlotNum;
    SetCountText(SlotIndex,0);
    FCoinTypeStructure CoinDataType = CoinSubsystem->GetCoinSlotCoinType(SlotIndex);
    SetCoinSlotCoinType(CoinDataType);

}
	

void UW_ShopCoinSlot::SetFrontWeaponImage(int32 WeaponID)
{
    FFaceData FrontWeaponData;
    if(DataSubsystem->TryGetWeapon(WeaponID,FrontWeaponData))
    {
        FrontWeaponImage->SetBrushFromTexture(FrontWeaponData.WeaponIcon);
    }
    else
    {
        FrontWeaponImage->SetBrushFromTexture(DefaultsTexture);
    }
}

void UW_ShopCoinSlot::SetBackWeaponImage(int32 WeaponID)
{

    FFaceData BackWeaponData;

    if(DataSubsystem->TryGetWeapon(WeaponID,BackWeaponData))
    {
        BackWeaponImage->SetBrushFromTexture(BackWeaponData.WeaponIcon);
    }
    else
    {
        BackWeaponImage->SetBrushFromTexture(DefaultsTexture);
    }
}

void UW_ShopCoinSlot::SetCoinSlotCoinType(FCoinTypeStructure CurrentCoinData)
{
    SetFrontWeaponImage(CurrentCoinData.FrontWeaponID);
    SetBackWeaponImage(CurrentCoinData.BackWeaponID);
}


void UW_ShopCoinSlot::ResetSlot()
{
    SetCountText(SlotIndex,0);

}

void UW_ShopCoinSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UW_ShopCoinSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
}