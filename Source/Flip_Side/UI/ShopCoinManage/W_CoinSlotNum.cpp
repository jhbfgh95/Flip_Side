// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_CoinManagePanelWidget.h"
#include "UI/ShopCoinManage/W_CoinSlotNum.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Components/TextBlock.h"
void UW_CoinSlotNum::NativeConstruct()
{
    Super::NativeConstruct();
    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

    CoinSubsystem->OnCoinSlotChange.AddDynamic(this,&UW_CoinSlotNum::SetSlotNumText);

    
    SetSlotNumText();

}

void UW_CoinSlotNum::NativeDestruct()
{
    CoinSubsystem->OnChangeSlotCoinSide.RemoveAll(this);
    Super::NativeDestruct();
}
	
void UW_CoinSlotNum::SetSlotNumText()
{
    CountText->SetText(FText::AsNumber(CoinSubsystem->GetCurrentSlotNum()));
}