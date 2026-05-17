// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_CoinSlotContainer.h"
#include "Components/TextBlock.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "UI/ShopCoinManage/W_CoinSlotWidget.h"

#define MAX_SLOT_NUM 10


void UW_CoinSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    ShopCoinSubsystem->OnCoinCountUpdate.AddDynamic(this , &UW_CoinSlotContainer::SetTotalCoinText);
    ShopCoinSubsystem->OnChangeCoinSlotCount.AddDynamic(this , &UW_CoinSlotContainer::UpdateSlotText);
    

    SlotAddButton->OnClicked.AddDynamic(this, &UW_CoinSlotContainer::AddCoinSlot);
    DecreaseSlotButton->OnClicked.AddDynamic(this, &UW_CoinSlotContainer::DecreaseCoinSlot);
    SetTotalCoinText(0, 0);
    InitCoinSlot();
}

void UW_CoinSlotContainer::NativeDestruct()
{
    ShopCoinSubsystem->OnCoinCountUpdate.RemoveAll(this);
    ShopCoinSubsystem->OnChangeCoinSlotCount.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_CoinSlotContainer::SetTotalCoinText(int32 ChangedSlotIndex, int32 Count)
{

    FString TextString = FString::Printf(TEXT("코인 개수 %d / 30"), ShopCoinSubsystem->GetTotalCoinCount());
    
    TotalCoinText->SetText(FText::FromString(TextString));
}


void UW_CoinSlotContainer::AddCoinSlot()
{

    UWidget* SlotWidget = SlotScroll->GetChildAt(CoinSlotCount);
    if(!SlotWidget)
        return;
    if(ShopCoinSubsystem->IncreaseCoinSlot(CoinSlotCount))
    {
        if(SlotBuyDescrip->GetVisibility() == ESlateVisibility::Visible)
            SlotBuyDescrip->SetVisibility(ESlateVisibility::Collapsed);

        SlotWidget->SetVisibility(ESlateVisibility::Visible);
        CoinSlotCount++;
    }


}

void UW_CoinSlotContainer::DecreaseCoinSlot()
{
    UWidget* SlotWidget = SlotScroll->GetChildAt(CoinSlotCount-1);
    if(!SlotWidget)
        return;
    if(ShopCoinSubsystem->DecreaseCoinSlot(CoinSlotCount-1))
    {
        SlotWidget->SetVisibility(ESlateVisibility::Collapsed);
        CoinSlotCount--;
    }
}


void UW_CoinSlotContainer::InitCoinSlot()
{
    for(int i =0; i<10; i++)
    {
        UUserWidget* CoinSlotUserWidget = CreateWidget<UUserWidget>(GetWorld(), CoinSlotWidget);
        if(Slot)
        {
            SlotScroll->AddChild(CoinSlotUserWidget);
            UW_CoinSlotWidget* SlotClass = Cast<UW_CoinSlotWidget>(CoinSlotUserWidget);
            CoinSlot.Add(SlotClass);
            SlotClass->InitSlot(i);
            SlotClass->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UW_CoinSlotContainer::UpdateSlotText(bool IsIncrease)
{
    int32 SlotCount = ShopCoinSubsystem->GetCurrentSlotCount();
    FString SlotText = FString::Printf(TEXT("%d / 10"), SlotCount);
    SlotInfoText->SetText(FText::FromString(SlotText));
}