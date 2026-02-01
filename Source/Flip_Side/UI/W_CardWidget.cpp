// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CardWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UW_CardWidget::NativeConstruct()
{
    Super::NativeConstruct();


}

void UW_CardWidget::InitCard(FCardData CardData)
{
    if(CardData.Icon != nullptr)
    {
        CardIconImage->SetBrushFromTexture(CardData.Icon);
    }
    CardTitle->SetText(FText::FromString(CardData.CardName));
    
    CardDescription->SetText(FText::FromString(CardData.Card_Description));
}
