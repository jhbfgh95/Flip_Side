// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopHandCardWidget.h"
#include "Subsystem/ShopCardWSubsystem.h"

void UW_ShopHandCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopCardSubSystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    ShopCardSubSystem->OnCardSelected.AddDynamic(this, &UW_ShopHandCardWidget::SelectedCard);
    this->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopHandCardWidget::SelectedCard(int32 Index, FCardData CardData)
{
    if(HandIndex == Index)
    {
        InitCard(CardData);
        this->SetVisibility(ESlateVisibility::Visible);
    }
}

void UW_ShopHandCardWidget::RemoveCard()
{
    this->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UW_ShopHandCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        UE_LOG(LogTemp, Log, TEXT("Left Click"));
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        UE_LOG(LogTemp, Log, TEXT("Right Click"));
        RemoveCard();
       ShopCardSubSystem->RemoveHandCard(HandIndex);
    }

    return FReply::Handled();
}