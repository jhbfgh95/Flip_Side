// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlotBuyButton.h"
#include "DataTypes/CoinDataTypes.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UW_ShopCoinSlotBuyButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    BuySlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotBuyButton::ClickBuySlotButton);
}

void UW_ShopCoinSlotBuyButton::InitWidget(int32 InBuySlotLevel, int32 InSlotPrice, int32 InBuySlotHp)
{
    BuySlotLevel = InBuySlotLevel;
    SlotPrice = InSlotPrice;
    BuySlotHp = InBuySlotHp;

    BuySlotHpTextBlock->SetText(FText::AsNumber(BuySlotHp));
    BuySlotPriceTextBlock->SetText(FText::AsNumber(SlotPrice));
}

void UW_ShopCoinSlotBuyButton::ClickBuySlotButton()
{
    OnClickedShopCoinSlotBuyButton.Broadcast(BuySlotLevel);
}
