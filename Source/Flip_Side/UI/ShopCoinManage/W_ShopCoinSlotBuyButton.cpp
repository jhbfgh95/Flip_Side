// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlotBuyButton.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Components/Button.h"


void UW_ShopCoinSlotBuyButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    BuySlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotBuyButton::ClickBuySlotButton);

}
	
void UW_ShopCoinSlotBuyButton::ClickBuySlotButton()
{
    CoinSubsystem->BuyCoinSlot(ButSlotLevel);
}