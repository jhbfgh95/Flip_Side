// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlot.h"

#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "WeaponDataTypes.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/Reply.h"
#include "UI/ShopCoinManage/ShopCoinSlotDragDropOperation.h"

void UW_ShopCoinSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    FrontWeaponImageButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::ClickFrontWeaponButton);
    
    BackWeaponImageButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::ClickBackWeaponButton);

    //SlotButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::PressSlotButton);

    IncreaseButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::BuyCoin);
    DecreaseButton->OnClicked.AddDynamic(this, &UW_ShopCoinSlot::SellCoin);


    BackGroundBorder->SetRenderOpacity(0.7f);
    
}
	
void UW_ShopCoinSlot::NativeDestruct()
{
    Super::NativeDestruct();
}


void UW_ShopCoinSlot::InitSlotWidget(int32 InSlotIndex)
{
    SlotIndex = InSlotIndex;
    SlotIndexText->SetText(FText::AsNumber(SlotIndex+1));
    SetCountText(0);
}


void UW_ShopCoinSlot::SetCoinSlot(int32 Hp)
{
    HPText->SetText(FText::AsNumber(Hp));
}

void UW_ShopCoinSlot::PressSlotButton()
{
    OnClickedShopCoinSlot.Broadcast(SlotIndex);
}

void UW_ShopCoinSlot::BuyCoin()
{
    OnBuyShopCoinSlotCoin.Broadcast(SlotIndex, 1);
}

void UW_ShopCoinSlot::SellCoin()
{
    OnSellShopCoinSlotCoin.Broadcast(SlotIndex, 1);
}

void UW_ShopCoinSlot::SetCountText(int32 Count)
{
    CoinCountText->SetText(FText::AsNumber(Count));
}

	
void UW_ShopCoinSlot::SetBackGround()
{
    /*
    if(CoinSubsystem->GetCurrentSlotNum()==SlotIndex)
        BackGroundBorder->SetRenderOpacity(1.2f);
    else
        BackGroundBorder->SetRenderOpacity(0.7f);*/
}

void UW_ShopCoinSlot::SetFrontWeaponImage(FFaceData InFrontCoinData)
{
    FButtonStyle ButtonStyle = FrontWeaponImageButton->GetStyle();
    UTexture2D* SetTexture;

    if(InFrontCoinData.WeaponID != -1)
    {
        SetTexture = InFrontCoinData.WeaponIcon;
    }
    else
    {
        SetTexture = DefaultsTexture;
    }
    
    ButtonStyle.Normal.SetResourceObject(SetTexture);
    ButtonStyle.Hovered.SetResourceObject(SetTexture);
    ButtonStyle.Pressed.SetResourceObject(SetTexture);

    FrontWeaponImageButton->SetStyle(ButtonStyle);
}

void UW_ShopCoinSlot::SetBackWeaponImage(FFaceData InBackCoinData)
{

    FButtonStyle ButtonStyle = BackWeaponImageButton->GetStyle();
    UTexture2D* SetTexture;
    
    if(InBackCoinData.WeaponID != -1)
    {
        SetTexture = InBackCoinData.WeaponIcon;
    }
    else
    {
        SetTexture = DefaultsTexture;
    }

    ButtonStyle.Normal.SetResourceObject(SetTexture);
    ButtonStyle.Hovered.SetResourceObject(SetTexture);
    ButtonStyle.Pressed.SetResourceObject(SetTexture);

    BackWeaponImageButton->SetStyle(ButtonStyle);
}



void UW_ShopCoinSlot::ClickFrontWeaponButton()
{
    OnClickedShopCoinSlotFrontCoin.Broadcast(SlotIndex);
}

void UW_ShopCoinSlot::ClickBackWeaponButton()
{
    OnClickedShopCoinSlotBackCoin.Broadcast(SlotIndex);
}

void UW_ShopCoinSlot::ResetSlot()
{
    SetCountText(0);
}

FReply UW_ShopCoinSlot::NativeOnMouseButtonDown(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        PressSlotButton();
        return UWidgetBlueprintLibrary::DetectDragIfPressed(
            InMouseEvent,
            this,
            EKeys::LeftMouseButton).NativeReply;
    }
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UW_ShopCoinSlot::NativeOnDragDetected(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent,
    UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    UShopCoinSlotDragDropOperation* DragOperation =
        NewObject<UShopCoinSlotDragDropOperation>(this);

    if (!IsValid(DragOperation))
    {
        return;
    }

    DragOperation->SourceSlotIndex = SlotIndex;
    DragOperation->TargetSlotIndex = SlotIndex;
    DragOperation->Pivot = EDragPivot::MouseDown;
    DragOperation->DefaultDragVisual = this;
    OutOperation = DragOperation;
}

bool UW_ShopCoinSlot::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UShopCoinSlotDragDropOperation* DragOperation =
        Cast<UShopCoinSlotDragDropOperation>(InOperation);

    if (!IsValid(DragOperation) ||
        DragOperation->SourceSlotIndex == INDEX_NONE ||
        DragOperation->TargetSlotIndex == INDEX_NONE ||
        DragOperation->SourceSlotIndex == DragOperation->TargetSlotIndex)
    {
        return false;
    }

    OnDropShopCoinSlot.Broadcast(
        DragOperation->SourceSlotIndex,
        DragOperation->TargetSlotIndex);
    return true;
}

void UW_ShopCoinSlot::NativeOnDragCancelled(
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

    if(IsValid(Cast<UShopCoinSlotDragDropOperation>(InOperation)))
    {
        OnCancelShopCoinSlotDrag.Broadcast();
    }
}

void UW_ShopCoinSlot::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    OnHoveredShopCoinSlot.Broadcast(SlotIndex);
}

void UW_ShopCoinSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    OnUnhoveredShopCoinSlot.Broadcast();
}
