// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlotContainer.h"
#include "Components/TextBlock.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "UI/ShopCoinManage/W_ShopCoinSlot.h"

#define MAX_SLOT_NUM 10


void UW_ShopCoinSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    ShopCoinSubsystem->OnCoinCountUpdate.AddDynamic(this , &UW_ShopCoinSlotContainer::SetTotalCoinText);
    ShopCoinSubsystem->OnChangeCoinSlotCount.AddDynamic(this , &UW_ShopCoinSlotContainer::UpdateSlotText);
    ShopCoinSubsystem->OnChangeCoinSlotCount.AddDynamic(this , &UW_ShopCoinSlotContainer::CoinSlotChangeAdaptor);

    //SlotAddButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotContainer::AddCoinSlot);
    //DecreaseSlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotContainer::DecreaseCoinSlot);
    SetTotalCoinText(0, 0);
    InitCoinSlot();
}

void UW_ShopCoinSlotContainer::NativeDestruct()
{
    ShopCoinSubsystem->OnCoinCountUpdate.RemoveAll(this);
    ShopCoinSubsystem->OnChangeCoinSlotCount.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_ShopCoinSlotContainer::SetTotalCoinText(int32 ChangedSlotIndex, int32 Count)
{
    /*
    FString TextString = FString::Printf(TEXT("코인 개수 %d / 30"), ShopCoinSubsystem->GetTotalCoinCount());
    
    TotalCoinText->SetText(FText::FromString(TextString));*/
}


void UW_ShopCoinSlotContainer::CoinSlotChangeAdaptor(bool IsIncreaseSlot)
{
    if(IsIncreaseSlot)
        AddCoinSlot();
    else
        DecreaseCoinSlot();
}

void UW_ShopCoinSlotContainer::AddCoinSlot()
{
    int32 AddSlotIndex = ShopCoinSubsystem->GetUnlockSlotLastIndex();
    UWidget* AddCoinSlot = CoinSlotBox->GetChildAt(AddSlotIndex);
    if(AddCoinSlot)
    {
        AddCoinSlot->SetVisibility(ESlateVisibility::Visible);
        CoinSlots[AddSlotIndex]->SetCoinSlot();
    }
        
}

void UW_ShopCoinSlotContainer::DecreaseCoinSlot()
{
    UWidget* AddCoinSlot = CoinSlotBox->GetChildAt(ShopCoinSubsystem->GetCurrentCoinSlotIndex()+1);
    AddCoinSlot->SetVisibility(ESlateVisibility::Collapsed);
}


void UW_ShopCoinSlotContainer::InitCoinSlot()
{
    for(int i =0; i<MAX_SLOT_NUM; i++)
    {
        UW_ShopCoinSlot* CoinSlotWidgetClass =Cast<UW_ShopCoinSlot>(CreateWidget<UUserWidget>(GetWorld(), CoinSlotWidget));
        if (CoinSlotWidgetClass)
        {
            CoinSlots.Add(CoinSlotWidgetClass);
            UVerticalBoxSlot* VSlot = CoinSlotBox->AddChildToVerticalBox(CoinSlotWidgetClass);

            if(VSlot)
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

            CoinSlotWidgetClass->InitSlot(i);
            CoinSlotWidgetClass->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

}

void UW_ShopCoinSlotContainer::UpdateSlotText(bool IsIncrease)
{
    /*
    int32 SlotCount = ShopCoinSubsystem->GetCurrentSlotCount();
    FString SlotText = FString::Printf(TEXT("%d / 10"), SlotCount);
    SlotInfoText->SetText(FText::FromString(SlotText));*/
}