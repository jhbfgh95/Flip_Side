// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_MoneyDisplay.h"
#include "Components/TextBlock.h"
void UW_MoneyDisplay::NativeConstruct()
{
    Super::NativeConstruct();
}

void UW_MoneyDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if(IsMoneyChanged)
    {

        CurrentMoneyText = FMath::FInterpTo(CurrentMoneyText, MoneyText, InDeltaTime, 10.0f);
        
        if (MoneyTextBlock)
        {
            MoneyTextBlock->SetText(FText::AsNumber(CurrentMoneyText));
        }

        if(CurrentMoneyText == MoneyText)
        {
            IsMoneyChanged = false;
        }
    }
}

void UW_MoneyDisplay::NativeDestruct()
{
    Super::NativeDestruct();
}

void UW_MoneyDisplay::SetMoneyText(int32 Money)
{
    IsMoneyChanged = true;
    MoneyText = Money;
}