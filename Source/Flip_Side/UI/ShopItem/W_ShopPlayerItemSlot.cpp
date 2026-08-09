// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopPlayerItemSlot.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "UI/ShopItem/ItemSlotDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/Reply.h"

void UW_ShopPlayerItemSlot::NativeConstruct()
{
    Super::NativeConstruct();
    ItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    MoneySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMoneyGISubsystem>();

    ItemSubsystem->OnItemBuy.AddDynamic(this, &UW_ShopPlayerItemSlot::SetItemWidget);


    ItemCountPlusButton->OnClicked.AddDynamic(this, &UW_ShopPlayerItemSlot::ClickItemCountPlusButton);
    ItemCountMinusButton->OnClicked.AddDynamic(this, &UW_ShopPlayerItemSlot::ClickItemCountMinusButton);
    ItemSellButton->OnClicked.AddDynamic(this, &UW_ShopPlayerItemSlot::ClickItemSellButton);
   UE_LOG(LogTemp, Warning, TEXT("개수 %d"),ItemSubsystem->GetPlayerItemNum());
}


void UW_ShopPlayerItemSlot::InitItemWidget(int32 InitIndex)
{
    PlayerInvenIndex = InitIndex;
    SetItemWidget(InitIndex);

}

void UW_ShopPlayerItemSlot::ClickItemSellButton()
{
    if(!ItemSubsystem) return;

    ItemSellCountTextBlock->SetText(FText::AsNumber(0));
    ItemSubsystem->SellItem(WidgetItemData, CurrentItemCount);
    CurrentItemCount = 0;
    int32 SameItemCount = ItemSubsystem->GetPlayerItem(PlayerInvenIndex).SameItemNum;
    ItemCountTextBlock->SetText(FText::AsNumber(SameItemCount));

    if(SameItemCount <= 0)
    {
        DeleteItemWidget();
    }
    
}

void UW_ShopPlayerItemSlot::ClickItemCountPlusButton()
{
    if(!ItemSubsystem) return;
    
    if(CurrentItemCount+1 <= ItemSubsystem->GetSameItemCountByItemID(WidgetItemData.ItemID))
        CurrentItemCount++;
    ItemSellCountTextBlock->SetText(FText::AsNumber(CurrentItemCount));
}
	
void UW_ShopPlayerItemSlot::ClickItemCountMinusButton()
{
    if(CurrentItemCount-1<0)
        return;

    CurrentItemCount--;
    ItemSellCountTextBlock->SetText(FText::AsNumber(CurrentItemCount));
}

void UW_ShopPlayerItemSlot::SetItemWidget(int32 BuyItemIndex)
{
    if(BuyItemIndex != PlayerInvenIndex)
        return;

    WidgetItemData = ItemSubsystem->GetPlayerItemData(PlayerInvenIndex);

    if(WidgetItemData.ItemID == -1)
    { 
        DeleteItemWidget();
        return;
    }
    ItemImage->SetBrushFromTexture(WidgetItemData.ItemIcon);
    ItemNameTextBlock->SetText(FText::FromString(WidgetItemData.ItemName));
    ItemCountTextBlock->SetText(FText::AsNumber(ItemSubsystem->GetPlayerItem(PlayerInvenIndex).SameItemNum));
    ItemPriceTextBlock->SetText(FText::AsNumber(WidgetItemData.Price));
    EmptySlotImage->SetVisibility(ESlateVisibility::Collapsed);
}


void UW_ShopPlayerItemSlot::DeleteItemWidget()
{
    ItemImage->SetBrushFromTexture(nullptr);
    ItemNameTextBlock->SetText(FText::GetEmpty());
    ItemCountTextBlock->SetText(FText::GetEmpty());
    ItemPriceTextBlock->SetText(FText::GetEmpty());
    EmptySlotImage->SetVisibility(ESlateVisibility::Visible);
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

void UW_ShopPlayerItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    ItemSubsystem->HoverItem(WidgetItemData);
}

void UW_ShopPlayerItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    ItemSubsystem->UnHoverItem();
}
