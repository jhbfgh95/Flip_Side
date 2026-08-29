// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopPlayerItemSlot.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "UI/ShopItem/ItemSlotDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/Reply.h"

void UW_ShopPlayerItemSlot::NativeConstruct()
{
    Super::NativeConstruct();
    ItemCountPlusButton->OnClicked.AddDynamic(this, &UW_ShopPlayerItemSlot::ClickItemCountPlusButton);
    ItemCountMinusButton->OnClicked.AddDynamic(this, &UW_ShopPlayerItemSlot::ClickItemCountMinusButton);
    ItemSellButton->OnClicked.AddDynamic(this, &UW_ShopPlayerItemSlot::ClickItemSellButton);
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
    ItemPriceTextBlock->SetText(FText::AsNumber(WidgetItemData.Price));
    ItemSellCountTextBlock->SetText(FText::AsNumber(0));
    EmptySlotImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopPlayerItemSlot::UpdateItemCount(int32 SameItemCount)
{
    CurrentItemCount=1;
    ItemSellCountTextBlock->SetText(FText::AsNumber(1));

    ItemCountTextBlock->SetText(FText::AsNumber(SameItemCount));
    ItemPriceTextBlock->SetText(FText::AsNumber(WidgetItemData.Price));
    WidgetSelectItemData.SameItemNum = SameItemCount;
}


void UW_ShopPlayerItemSlot::ClickItemSellButton()
{
    OnSellItem.Broadcast(PlayerInvenIndex, WidgetItemData.ItemID, CurrentItemCount);
}

void UW_ShopPlayerItemSlot::ClickItemCountPlusButton()
{
    if(WidgetSelectItemData.SameItemNum < CurrentItemCount + 1)
        return;
    
    UE_LOG(LogTemp, Warning, TEXT("개수증가"));
    CurrentItemCount++;
    ItemPriceTextBlock->SetText(FText::AsNumber(CurrentItemCount* WidgetItemData.Price));
    ItemSellCountTextBlock->SetText(FText::AsNumber(CurrentItemCount));
}
	
void UW_ShopPlayerItemSlot::ClickItemCountMinusButton()
{
    if(CurrentItemCount-1<0)
        return;
    
        UE_LOG(LogTemp, Warning, TEXT("개수감소"));
    CurrentItemCount--;
    ItemPriceTextBlock->SetText(FText::AsNumber(CurrentItemCount* WidgetItemData.Price));
    ItemSellCountTextBlock->SetText(FText::AsNumber(CurrentItemCount));
}


void UW_ShopPlayerItemSlot::DeleteItemWidget()
{
    CurrentItemCount = 0;
    ItemSellCountTextBlock->SetText(FText::AsNumber(0));
    ItemImage->SetBrushFromTexture(nullptr);
    ItemNameTextBlock->SetText(FText::GetEmpty());
    ItemCountTextBlock->SetText(FText::GetEmpty());
    ItemPriceTextBlock->SetText(FText::GetEmpty());
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

    // 드래그 이미지의 중앙이 마우스를 따라감
    DragOperation->Pivot = EDragPivot::CenterCenter;

    DragOperation->DefaultDragVisual = this;

    OutOperation = DragOperation;
}

