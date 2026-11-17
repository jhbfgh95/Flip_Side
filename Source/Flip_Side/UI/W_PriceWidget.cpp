// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_PriceWidget.h"
#include "Components/TextBlock.h"

void UW_PriceWidget::SetPriceText(int32 Price)
{
    PriceText->SetText(FText::Format(
        NSLOCTEXT("PriceWidget", "GoldAmount", "{0} Gold"), FText::AsNumber(Price)));
}
