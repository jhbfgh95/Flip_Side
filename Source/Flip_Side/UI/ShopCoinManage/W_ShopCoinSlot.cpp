// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlot.h"

#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "WeaponDataTypes.h"
#include "Components/Border.h"

void UW_ShopCoinSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    FrontWeaponImageButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::ClickFrontWeaponButton);
    
    BackWeaponImageButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::ClickBackWeaponButton);

    SlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::PressSlotButton);

    IncreaseButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::BuyCoin);
    DecreaseButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::SellCoin);


    BackGroundBorder->SetRenderOpacity(0.7f);
    
}
	
void UW_ShopCoinSlot::NativeDestruct()
{
    Super::NativeDestruct();
}


void UW_ShopCoinSlot::InitSlotWidget(int32 InSlotIndex)
{
    SlotIndex = InSlotIndex;
    SlotIndexText->SetText(FText::AsNumber(SlotIndex+1));
}


void UW_ShopCoinSlot::SetCoinSlot(int32 Hp)
{
    HPText->SetText(FText::AsNumber(Hp));
}

void UW_ShopCoinSlot::PressSlotButton()
{
    OnClickedShopCoinSlot.Broadcast(SlotIndex);
}

void UW_ShopCoinSlot::BuyCoin()
{
    OnBuyShopCoinSlotCoin.Broadcast(SlotIndex, 1);
}

void UW_ShopCoinSlot::SellCoin()
{
    OnSellShopCoinSlotCoin.Broadcast(SlotIndex, 1);
}

void UW_ShopCoinSlot::SetCountText(int32 Count)
{
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

void UW_ShopCoinSlot::SetFrontWeaponImage(FFaceData InFrontCoinData)
{
    FButtonStyle ButtonStyle = FrontWeaponImageButton->GetStyle();
    UTexture2D* SetTexture;

    if(InFrontCoinData.WeaponID != -1)
    {
        SetTexture = InFrontCoinData.WeaponIcon;
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

void UW_ShopCoinSlot::SetBackWeaponImage(FFaceData InBackCoinData)
{

    FButtonStyle ButtonStyle = BackWeaponImageButton->GetStyle();
    UTexture2D* SetTexture;
    
    if(InBackCoinData.WeaponID != -1)
    {
        SetTexture = InBackCoinData.WeaponIcon;
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



void UW_ShopCoinSlot::ClickFrontWeaponButton()
{
    OnClickedShopCoinSlotFrontCoin.Broadcast(SlotIndex);
}

void UW_ShopCoinSlot::ClickBackWeaponButton()
{
    OnClickedShopCoinSlotBackCoin.Broadcast(SlotIndex);
}

void UW_ShopCoinSlot::ResetSlot()
{
    SetCountText(0);
}

void UW_ShopCoinSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    OnHoveredShopCoinSlot.Broadcast(SlotIndex);
}

void UW_ShopCoinSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    OnUnhoveredShopCoinSlot.Broadcast();
}
