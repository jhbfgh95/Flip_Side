// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardWidget.h"

#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"
void UW_ShopCardWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    ShopCardSubsystem->OnUnlockCard.AddDynamic(this, &UW_ShopCardWidget::UnlockCard);
}

void UW_ShopCardWidget::NativeDestruct()
{
    Super::NativeDestruct();
}


FReply UW_ShopCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ShopCardSubsystem->SelectCard(CardData);
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        ShopCardSubsystem->UnSelectPlayerCard(CardData);
        //PlayAnimation(UnSelectAnim);
    }

    return FReply::Handled();
}

void UW_ShopCardWidget::InitShopCard(FCardData CardDataValue, bool IsUnlock)
{

    CardData = CardDataValue;

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
    if(UnlockCardID == CardData.CardID)
    {
        HideLockIamge();
    }
}

	
void UW_ShopCardWidget::HideLockIamge()
{
    LockImage->SetVisibility(ESlateVisibility::Hidden);
}