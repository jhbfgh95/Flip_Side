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

    SlotAddButton->OnClicked.AddDynamic(this, &UW_CoinSlotContainer::AddCoinSlot);
    SetTotalCoinText(0, 0);
}

void UW_CoinSlotContainer::NativeDestruct()
{
     ShopCoinSubsystem->OnCoinCountUpdate.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_CoinSlotContainer::SetTotalCoinText(int32 ChangedSlotIndex, int32 Count)
{

    FString TextString = FString::Printf(TEXT("%d / 30"), ShopCoinSubsystem->GetTotalCoinCount());
    
    TotalCoinText->SetText(FText::FromString(TextString));
}


void UW_CoinSlotContainer::AddCoinSlot()
{
    if(MAX_SLOT_NUM<= CoinSlot.Num())
        return;
    
    UUserWidget* CoinSlotUserWidget = CreateWidget<UUserWidget>(GetWorld(), CoinSlotWidget);
    
    if(Slot)
    {
        SlotScroll->AddChild(CoinSlotUserWidget);
        UW_CoinSlotWidget* SlotClass = Cast<UW_CoinSlotWidget>(CoinSlotUserWidget);
        CoinSlot.Add(SlotClass);
        SlotClass->InitSlot(CoinSlot.Num()-1);
    }
    ShopCoinSubsystem->UnlockCoinSlot(CoinSlot.Num()-1);
}