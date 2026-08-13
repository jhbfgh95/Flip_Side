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

    FrontWeaponImageButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::ClickFrontWeaponButton);
    
    BackWeaponImageButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::ClickBackWeaponButton);

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
    SlotIndexText->SetText(FText::AsNumber(SlotIndex+1));
    SetCountText(SlotIndex,0);
    FCoinTypeStructure CoinDataType = CoinSubsystem->GetCoinSlotCoinType(SlotIndex);
    SetCoinSlotCoinType(CoinDataType);

}
	

void UW_ShopCoinSlot::SetFrontWeaponImage(int32 WeaponID)
{
    FFaceData FrontWeaponData;
    FButtonStyle ButtonStyle = FrontWeaponImageButton->GetStyle();
    UTexture2D* SetTexture;

    if(DataSubsystem->TryGetWeapon(WeaponID,FrontWeaponData))
    {
        SetTexture = FrontWeaponData.WeaponIcon;
    }
    else
    {
        SetTexture = DefaultsTexture;
    }
    
    ButtonStyle.Normal.SetResourceObject(SetTexture);
    ButtonStyle.Hovered.SetResourceObject(SetTexture);
    ButtonStyle.Pressed.SetResourceObject(SetTexture);

    FrontWeaponImageButton->SetStyle(ButtonStyle);
}

void UW_ShopCoinSlot::SetBackWeaponImage(int32 WeaponID)
{

    FFaceData BackWeaponData;
    FButtonStyle ButtonStyle = FrontWeaponImageButton->GetStyle();
    UTexture2D* SetTexture;
    
    if(DataSubsystem->TryGetWeapon(WeaponID,BackWeaponData))
    {
        SetTexture = BackWeaponData.WeaponIcon;
    }
    else
    {
        SetTexture = DefaultsTexture;
    }

    ButtonStyle.Normal.SetResourceObject(SetTexture);
    ButtonStyle.Hovered.SetResourceObject(SetTexture);
    ButtonStyle.Pressed.SetResourceObject(SetTexture);

    BackWeaponImageButton->SetStyle(ButtonStyle);
}

void UW_ShopCoinSlot::SetCoinSlotCoinType(FCoinTypeStructure CurrentCoinData)
{
    SetFrontWeaponImage(CurrentCoinData.FrontWeaponID);
    SetBackWeaponImage(CurrentCoinData.BackWeaponID);
}

	
void UW_ShopCoinSlot::SetCoinSlot()
{
    FCoinTypeStructure CoinData = CoinSubsystem->GetCoinSlotCoinType(SlotIndex);
    SetCoinSlotCoinType(CoinData);
    int32 price;
    int32 hp;
    DataSubsystem->GetCoinSlotLevelStats(CoinData, price, hp);
    HPText->SetText(FText::AsNumber(hp));

}


void UW_ShopCoinSlot::ClickFrontWeaponButton()
{
    UE_LOG(LogTemp, Warning, TEXT("현재 인덱스 %d, 슬롯 인덱스 %d "), CoinSubsystem->GetCurrentCoinSlotIndex(),SlotIndex);
    if(SlotIndex != CoinSubsystem->GetCurrentCoinSlotIndex())
    {
        UE_LOG(LogTemp, Warning, TEXT("인덱스 %d"), CoinSubsystem->GetCurrentCoinSlotIndex());
        CoinSubsystem->SelectCoin(SlotIndex);
    }
    
    CoinSubsystem->ChangeCoinSide(true);
}

void UW_ShopCoinSlot::ClickBackWeaponButton()
{
    if(SlotIndex != CoinSubsystem->GetCurrentCoinSlotIndex())
        CoinSubsystem->SelectCoin(SlotIndex);
        
    CoinSubsystem->ChangeCoinSide(false);
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