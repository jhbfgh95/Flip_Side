// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopHandCardWidget.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Components/CanvasPanel.h"

#include "Components/CanvasPanelSlot.h"
void UW_ShopHandCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopCardSubSystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    ShopCardSubSystem->OnCardSelected.AddDynamic(this, &UW_ShopHandCardWidget::SelectedCard);
    this->SetVisibility(ESlateVisibility::Collapsed);

    if (RemoveCardAnim)
    {
        FWidgetAnimationDynamicEvent EndEvent;
        EndEvent.BindDynamic(this, &UW_ShopHandCardWidget::OnUnSelectCardAnimFinished);
        BindToAnimationFinished(RemoveCardAnim, EndEvent);

    }
    CanControl = true;
}

void UW_ShopHandCardWidget::SelectedCard(int32 Index, FCardData CardData)
{
    
    if(HandIndex == Index)
    {
        InitCard(CardData);
        this->SetVisibility(ESlateVisibility::Visible);
        PlayCardAnim(AddCardAnim);
    }
}

void UW_ShopHandCardWidget::RemoveCard()
{
    this->SetVisibility(ESlateVisibility::Collapsed);
    CanControl = true;
}

FReply UW_ShopHandCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        UE_LOG(LogTemp, Log, TEXT("Left Click"));
        if (SelectCardAnim)
        {
            //PlayAnimation(SelectCardAnim);
        }
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        UE_LOG(LogTemp, Log, TEXT("Right Click"));
        if (RemoveCardAnim)
        {
            PlayCardAnim(RemoveCardAnim);
            CanControl = false;
        }
    }

    return FReply::Handled();
}

void UW_ShopHandCardWidget::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
        PlayCardAnim(SelectCardAnim);
}

void UW_ShopHandCardWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    PlayCardAnim(UnSelectCardAnim);
}

void UW_ShopHandCardWidget::OnUnSelectCardAnimFinished()
{
    RemoveCard();
    ShopCardSubSystem->RemoveHandCard(HandIndex);
}

void UW_ShopHandCardWidget::PlayCardAnim(UWidgetAnimation* Anim)
{
    if(CanControl)
    {
        PlayAnimation(Anim);
    }
}