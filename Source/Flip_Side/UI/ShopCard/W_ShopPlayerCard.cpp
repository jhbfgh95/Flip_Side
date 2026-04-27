// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerCard.h"


#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"
void UW_ShopPlayerCard::NativeConstruct()
{
    Super::NativeConstruct();
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
        ShopCardSubsystem->SelectPlayerCard(CardData);
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        ShopCardSubsystem->UnSelectPlayerCard(CardData);
        //PlayAnimation(UnSelectAnim);
    }

    return FReply::Handled();
}

void UW_ShopPlayerCard::InitShopCard(FCardData CardDataValue, bool IsUnlock)
{
    if(!IsInitState)
    {
            PlayAnimation(UnSelectAnim, UnSelectAnim->GetEndTime(), 1, EUMGSequencePlayMode::Forward);
            IsInitState = true;
    }
    else
    {
        if(CardData.CardID == -1)
        {
            if(CardDataValue.CardID != -1)
                PlayAnimation(SelectAnim);
            
                
        }
        else
        {
            if(CardDataValue.CardID == -1)
                PlayAnimation(UnSelectAnim);
            else if(CardDataValue.CardID != CardData.CardID)
                PlayAnimation(ChangeCardAnim);
        }
    }
    

    CardData = CardDataValue;


    InitCard(CardDataValue);


}
