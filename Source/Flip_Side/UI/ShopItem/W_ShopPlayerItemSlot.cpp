// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopPlayerItemSlot.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "UI/ShopItem/ItemSlotDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/Reply.h"

void UW_ShopPlayerItemSlot::NativeConstruct()
{
    Super::NativeConstruct();
}

void UW_ShopPlayerItemSlot::InitItemWidget(int32 ItemIndex, FItemData InItemData, FSelectItem InSelectItemData)
{
    PlayerInvenIndex = ItemIndex;
    SetItemWidget(InItemData, InSelectItemData);
}

void UW_ShopPlayerItemSlot::SetItemWidget(FItemData InItemData, FSelectItem InSelectItemData)
{
    WidgetItemData = InItemData;
    WidgetSelectItemData = InSelectItemData;

    if(WidgetItemData.ItemID == -1)
    { 
        DeleteItemWidget();
        return;
    }

    ItemImage->SetBrushFromTexture(WidgetItemData.ItemIcon);
    ItemNameTextBlock->SetText(FText::FromString(WidgetItemData.ItemName));
    ItemCountTextBlock->SetText(FText::AsNumber(InSelectItemData.SameItemNum));
    EmptySlotImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopPlayerItemSlot::UpdateItemCount(int32 SameItemCount)
{
    ItemCountTextBlock->SetText(FText::AsNumber(SameItemCount));
    WidgetSelectItemData.SameItemNum = SameItemCount;
}


void UW_ShopPlayerItemSlot::DeleteItemWidget()
{
    ItemImage->SetBrushFromTexture(nullptr);
    ItemNameTextBlock->SetText(FText::GetEmpty());
    ItemCountTextBlock->SetText(FText::GetEmpty());
    EmptySlotImage->SetVisibility(ESlateVisibility::Visible);
}

void UW_ShopPlayerItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    OnHoveredSlot.Broadcast(PlayerInvenIndex);
}

void UW_ShopPlayerItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    OnUnhoveredSlot.Broadcast();
}


FReply UW_ShopPlayerItemSlot::NativeOnMouseButtonDown(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        OnRequestSellPopup.Broadcast(this,PlayerInvenIndex);
        return UWidgetBlueprintLibrary::DetectDragIfPressed(
            InMouseEvent,
            this,
            EKeys::LeftMouseButton
        ).NativeReply;
    }

    return Super::NativeOnMouseButtonDown(
        InGeometry,
        InMouseEvent
    );
}

void UW_ShopPlayerItemSlot::NativeOnDragDetected(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent,
    UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(
        InGeometry,
        InMouseEvent,
        OutOperation
    );

    UItemSlotDragDropOperation* DragOperation =
        NewObject<UItemSlotDragDropOperation>(this);

    if (!IsValid(DragOperation))
    {
        return;
    }

    DragOperation->DraggedSlotWidget = this;

    DragOperation->Pivot = EDragPivot::CenterCenter;

    DragOperation->DefaultDragVisual = this;

    OutOperation = DragOperation;
}

