// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlotBuyButton.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UW_ShopCoinSlotBuyButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    BuySlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotBuyButton::ClickBuySlotButton);

    FCoinTypeStructure InitCoinData;
    InitCoinData.Level = BuySlotLevel;
    DataManager->GetCoinSlotLevelStats(InitCoinData,SlotPrice,BuySlotHp);

    BuySlotHpTextBlock->SetText(FText::AsNumber(BuySlotHp));
    BuySlotPriceTextBlock->SetText(FText::AsNumber(SlotPrice));
}
	
void UW_ShopCoinSlotBuyButton::ClickBuySlotButton()
{
    CoinSubsystem->BuyCoinSlot(BuySlotLevel);
}