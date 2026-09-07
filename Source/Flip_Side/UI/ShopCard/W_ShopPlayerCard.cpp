// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerCard.h"


#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Animation/WidgetAnimation.h"
void UW_ShopPlayerCard::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
}

void UW_ShopPlayerCard::NativeDestruct()
{
    Super::NativeDestruct();
}


FReply UW_ShopPlayerCard::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        if(CardData.CardID != -1)
        {
            //ShopCardSubsystem->SelectPlayerCard(CardData);
        }
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        RClickCard();
    }

    return FReply::Handled();
}

void UW_ShopPlayerCard::InitPlayerCard(FCardData CardDataValue)
{
    /*
    CardData = CardDataValue;

    if(CardData.CardID == -1)
    {
        CardImage->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        CardImage->SetVisibility(ESlateVisibility::Visible);
    }
    InitCard(CardDataValue);

*/
}

void UW_ShopPlayerCard::RClickCard()
{
    //ShopCardSubsystem->UnSelectPlayerCard(CardData);
}