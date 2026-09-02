// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardWidget.h"

#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"
void UW_ShopCardWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UW_ShopCardWidget::NativeDestruct()
{
    Super::NativeDestruct();
}


FReply UW_ShopCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        LeftClickCard();
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        //ShopCardSubsystem->UnSelectPlayerCard(CurrentCardData);
        //PlayAnimation(UnSelectAnim);
    }

    return FReply::Handled();
}

void UW_ShopCardWidget::InitShopCard(FCardData CardDataValue, bool IsUnlock)
{

    CurrentCardData = CardDataValue;

    if(IsUnlock)
    {
        HideLockIamge();
    }

    InitCard(CardDataValue);


}



void UW_ShopCardWidget::SetCardSelect(int32 SelctCardID)
{
    
}
	

void UW_ShopCardWidget::UnlockCard(int32 UnlockCardID)
{
    if(UnlockCardID == CurrentCardData.CardID)
    {
        HideLockIamge();
    }
}

	
void UW_ShopCardWidget::HideLockIamge()
{
    LockImage->SetVisibility(ESlateVisibility::Hidden);
}

void UW_ShopCardWidget::LeftClickCard()
{
}
	
void UW_ShopCardWidget::SetCardData(FCardData CardData)
{
    SetVisibility(ESlateVisibility::Visible);
    InitCard(CardData);
}
	
void UW_ShopCardWidget::UnsetCardData()
{
    SetVisibility(ESlateVisibility::Collapsed);
}
