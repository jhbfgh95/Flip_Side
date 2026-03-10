// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockCard/W_UnlockCardWidget.h"

#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/ShopLevel/ShopUnlockCardWSubsystem.h"
#include "Components/Image.h"

void UW_UnlockCardWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    ShopUnlockCardSubsystem = GetWorld()->GetSubsystem<UShopUnlockCardWSubsystem>();
    ShopUnlockCardSubsystem->OnSelectUnlockCard.AddDynamic(this, &UW_UnlockCardWidget::SetCardSelect);
    ShopUnlockCardSubsystem->OnUnlockSelectCard.AddDynamic(this, &UW_UnlockCardWidget::SetUnlockCard);
}

void UW_UnlockCardWidget::NativeDestruct()
{
    ShopUnlockCardSubsystem->OnSelectUnlockCard.RemoveAll(this);
    ShopUnlockCardSubsystem->OnUnlockSelectCard.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_UnlockCardWidget::InitUnlockCard(FCardData CardData)
{
    if(UnlockSubsystem->IsCardUnlockByID(CardData.CardID))
    {
        LockImage->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        LockImage->SetVisibility(ESlateVisibility::Visible);
    }
    InitCard(CardData);
    CardData.CardID = 0;
    UnlockCardData = CardData;
}

FReply UW_UnlockCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        
        UE_LOG(LogTemp, Log, TEXT("Left Click"));
        ShopUnlockCardSubsystem->SelectCard(UnlockCardData.CardID);
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        ShopUnlockCardSubsystem->UnSelectCard();
    }

    return FReply::Handled();
}

void UW_UnlockCardWidget::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    //위치 이동 
}

void UW_UnlockCardWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

}

void UW_UnlockCardWidget::SetCardSelect(int32 SelctCardID)
{
    if(SelctCardID == UnlockCardData.CardID)
    {
        PlayAnimation(SelectAnim);
    }
    else
    {
        PlayAnimation(UnSelectAnim);
    }
}
	

void UW_UnlockCardWidget::SetUnlockCard(int32 UnlockCardID)
{
    if(UnlockCardData.CardID == UnlockCardID)
    {
        //if(!UnlockSubsystem->IsCardUnlockByID(UnlockCardData.CardID))
        {
            PlayAnimation(UnlockCardAnim);
        }
        
    }
}
	
void UW_UnlockCardWidget::HideLockIamge()
{
    LockImage->SetVisibility(ESlateVisibility::Hidden);
}