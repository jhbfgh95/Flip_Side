// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockCard/W_UnlockCardModeWidget.h"
#include "UI/ShopUnlockCard/W_UnlockCardWidget.h"

#include "Subsystem/ShopLevel/ShopUnlockCardWSubsystem.h"
void UW_UnlockCardModeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopUnlockCardSubsystem = GetWorld()->GetSubsystem<UShopUnlockCardWSubsystem>();

}


void UW_UnlockCardModeWidget::InitUnlockCards()
{
    UnlockCardWidget1->InitCard(ShopUnlockCardSubsystem->GetCardDataByIndex(CurrentPage));
    UnlockCardWidget2->InitCard(ShopUnlockCardSubsystem->GetCardDataByIndex(CurrentPage + 1));
    UnlockCardWidget3->InitCard(ShopUnlockCardSubsystem->GetCardDataByIndex(CurrentPage+ 2));
    UnlockCardWidget4->InitCard(ShopUnlockCardSubsystem->GetCardDataByIndex(CurrentPage + 3));
}

void UW_UnlockCardModeWidget::ChangeCardsLeft()
{
    ShopUnlockCardSubsystem->ChangeCardsLeft();
}

void UW_UnlockCardModeWidget::ChangeCardsRight()
{
    ShopUnlockCardSubsystem->ChangeCardsRight();
}