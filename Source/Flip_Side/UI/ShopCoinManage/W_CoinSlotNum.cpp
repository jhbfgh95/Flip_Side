// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_CoinSlotNum.h"
#include "UI/ShopCoinManage/W_CoinManagePanelWidget.h"
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
    if(CoinSubsystem->GetCurrentSlotNum() ==0)
    {
        if(!CoinSubsystem->GetCurrentCoinUnlock())
        {
            CountText->SetText(FText::FromString(TEXT("X")));
        }
        else
            CountText->SetText(FText::AsNumber(CoinSubsystem->GetCurrentSlotNum()+1));
    }
    else
        CountText->SetText(FText::AsNumber(CoinSubsystem->GetCurrentSlotNum()+1));
    
}