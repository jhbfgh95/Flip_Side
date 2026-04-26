// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopSelectCardCancelImage.h"

#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"

void UW_ShopSelectCardCancelImage::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
}


FReply UW_ShopSelectCardCancelImage::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ShopCardSubsystem->UnSelectCard();
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        ShopCardSubsystem->UnSelectCard();
    }

    return FReply::Handled();
}
