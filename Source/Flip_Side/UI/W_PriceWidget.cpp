// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_PriceWidget.h"
#include "Components/TextBlock.h"

void UW_PriceWidget::SetPriceText(int32 Price)
{
    FString text = FString::Printf(TEXT("%d Gold"), Price);
    PriceText->SetText(FText::FromString(text));
}