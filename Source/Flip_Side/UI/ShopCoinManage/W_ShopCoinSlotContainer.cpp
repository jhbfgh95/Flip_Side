// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlotContainer.h"
#include "Components/TextBlock.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "UI/ShopCoinManage/W_ShopCoinSlot.h"
#include "UI/ShopCoinManage/ShopCoinPresenter.h"

#define MAX_SLOT_NUM 10


void UW_ShopCoinSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();

    //SlotAddButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotContainer::AddCoinSlot);
    //DecreaseSlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotContainer::DecreaseCoinSlot);
    SetTotalCoinText(0, 0);
}

void UW_ShopCoinSlotContainer::NativeDestruct()
{
    Super::NativeDestruct();
}

void UW_ShopCoinSlotContainer::SetTotalCoinText(int32 ChangedSlotIndex, int32 Count)
{
    /*
    FString TextString = FString::Printf(TEXT("코인 개수 %d / 30"), ShopCoinSubsystem->GetTotalCoinCount());
    
    TotalCoinText->SetText(FText::FromString(TextString));*/
}


void UW_ShopCoinSlotContainer::AddCoinSlot(int32 AddSlotIndex, int32 AddSlotHP)
{
    if(CoinSlots.IsValidIndex(AddSlotIndex))
    {
        CoinSlots[AddSlotIndex]->SetVisibility(ESlateVisibility::Visible);
        CoinSlots[AddSlotIndex]->SetCoinSlot(AddSlotHP);
    }
        
}

void UW_ShopCoinSlotContainer::RemoveCoinSlot(int32 RemoveSlotIndex)
{
    CoinSlots[RemoveSlotIndex]->SetVisibility(ESlateVisibility::Collapsed);
}


void UW_ShopCoinSlotContainer::InitWidget()
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

            CoinSlotWidgetClass->InitSlotWidget(i);
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
