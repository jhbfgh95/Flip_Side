// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlotContainer.h"
#include "Components/TextBlock.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "UI/ShopCoinManage/W_ShopCoinSlot.h"
#include "UI/ShopCoinManage/ShopCoinSlotDragDropOperation.h"
#include "UI/ShopCoinManage/ShopCoinPresenter.h"

#define MAX_SLOT_NUM 10


void UW_ShopCoinSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();

    //SlotAddButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotContainer::AddCoinSlot);
    //DecreaseSlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlotContainer::DecreaseCoinSlot);
    SetTotalCoinText(0);
    SetSlotInfoText(0);
}

void UW_ShopCoinSlotContainer::NativeDestruct()
{
    Super::NativeDestruct();
}

bool UW_ShopCoinSlotContainer::NativeOnDragOver(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("드래그중"));
    UShopCoinSlotDragDropOperation* DragOperation =
        Cast<UShopCoinSlotDragDropOperation>(InOperation);

    if(!IsValid(DragOperation) ||
        !CoinSlots.IsValidIndex(DragOperation->SourceSlotIndex))
    {
        return Super::NativeOnDragOver(
            InGeometry,
            InDragDropEvent,
            InOperation);
    }

    UW_ShopCoinSlot* SourceSlot = CoinSlots[DragOperation->SourceSlotIndex];
    if(!IsValid(SourceSlot))
    {
        return false;
    }

    int32 VisibleSlotCount = 0;
    int32 TargetPosition = 0;
    const float MouseY = InDragDropEvent.GetScreenSpacePosition().Y;

    for(int32 ChildIndex = 0; ChildIndex < CoinSlotBox->GetChildrenCount(); ++ChildIndex)
    {
        UW_ShopCoinSlot* ChildSlot =
            Cast<UW_ShopCoinSlot>(CoinSlotBox->GetChildAt(ChildIndex));

        if(!IsValid(ChildSlot) || !ChildSlot->IsVisible())
        {
            continue;
        }

        ++VisibleSlotCount;

        if(ChildSlot == SourceSlot)
        {
            continue;
        }

        const FGeometry& ChildGeometry = ChildSlot->GetCachedGeometry();
        const float ChildCenterY = ChildGeometry.LocalToAbsolute(
            FVector2D(0.0f, ChildGeometry.GetLocalSize().Y * 0.5f)).Y;

        if(MouseY > ChildCenterY)
        {
            ++TargetPosition;
        }
    }

    if(VisibleSlotCount <= 0)
    {
        return false;
    }

    TargetPosition = FMath::Clamp(TargetPosition, 0, VisibleSlotCount - 1);
    DragOperation->TargetSlotIndex = TargetPosition;

    if(CoinSlotBox->GetChildIndex(SourceSlot) != TargetPosition)
    {
        TArray<TObjectPtr<UW_ShopCoinSlot>> PreviewOrder;
        PreviewOrder.Reserve(CoinSlotBox->GetChildrenCount());

        for(int32 ChildIndex = 0; ChildIndex < CoinSlotBox->GetChildrenCount(); ++ChildIndex)
        {
            if(UW_ShopCoinSlot* ChildSlot =
                Cast<UW_ShopCoinSlot>(CoinSlotBox->GetChildAt(ChildIndex)))
            {
                PreviewOrder.Add(ChildSlot);
            }
        }

        PreviewOrder.Remove(SourceSlot);
        PreviewOrder.Insert(SourceSlot, TargetPosition);
        RebuildSlotBox(PreviewOrder);
    }

    return true;
}

void UW_ShopCoinSlotContainer::SetTotalCoinText(int32 TotalCoinCount)
{
    if (TotalCoinText)
    {
        TotalCoinText->SetText(FText::FromString(FString::Printf(TEXT("%d/ 30"), TotalCoinCount)));
    }
}

void UW_ShopCoinSlotContainer::SetSlotInfoText(int32 SlotCount)
{
    if (SlotInfoText)
    {
        SlotInfoText->SetText(FText::FromString(FString::Printf(TEXT("%d/ %d"), SlotCount, MAX_SLOT_NUM)));
    }
}


void UW_ShopCoinSlotContainer::AddCoinSlot(int32 AddSlotIndex, int32 AddSlotHP)
{
    if(CoinSlots.IsValidIndex(AddSlotIndex))
    {
        CoinSlots[AddSlotIndex]->SetVisibility(ESlateVisibility::Visible);
        CoinSlots[AddSlotIndex]->SetCoinSlot(AddSlotHP);
    }
    CoinSlotScrollBox->ScrollWidgetIntoView(
            CoinSlots[AddSlotIndex],
            true,
            EDescendantScrollDestination::IntoView,
            10.0f
        );
}

void UW_ShopCoinSlotContainer::RemoveCoinSlot(int32 RemoveSlotIndex)
{
    CoinSlots[RemoveSlotIndex]->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopCoinSlotContainer::ResetSlotPreview()
{
    RebuildSlotBox(CoinSlots);
}

void UW_ShopCoinSlotContainer::RebuildSlotBox(
    const TArray<TObjectPtr<UW_ShopCoinSlot>>& OrderedSlots)
{
    CoinSlotBox->ClearChildren();

    for(UW_ShopCoinSlot* CoinSlot : OrderedSlots)
    {
        if(!IsValid(CoinSlot))
        {
            continue;
        }

        UVerticalBoxSlot* VerticalSlot = CoinSlotBox->AddChildToVerticalBox(CoinSlot);
        if(IsValid(VerticalSlot))
        {
            VerticalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
            VerticalSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
        }
    }
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
            {
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
                VSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
            }
            CoinSlotWidgetClass->InitSlotWidget(i);
            CoinSlotWidgetClass->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}
