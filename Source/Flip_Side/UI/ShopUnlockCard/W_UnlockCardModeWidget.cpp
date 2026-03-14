// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockCard/W_UnlockCardModeWidget.h"
#include "UI/ShopUnlockCard/W_UnlockCardWidget.h"

#include "Subsystem/ShopLevel/ShopUnlockCardWSubsystem.h"

#include "Components/Button.h"
void UW_UnlockCardModeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopUnlockCardSubsystem = GetWorld()->GetSubsystem<UShopUnlockCardWSubsystem>();
    InitUnlockCards();
    
    LeftPageButton->OnClicked.AddDynamic(this, &UW_UnlockCardModeWidget::ChangeCardsLeft);
    RightPageButton->OnClicked.AddDynamic(this, &UW_UnlockCardModeWidget::ChangeCardsRight);

}   


void UW_UnlockCardModeWidget::InitUnlockCards()
{
    UnlockCardWidget1->InitCardShowIndex(0);
    UnlockCardWidget2->InitCardShowIndex(1);
    UnlockCardWidget3->InitCardShowIndex(2);
    UnlockCardWidget4->InitCardShowIndex(3);

     UnlockCardWidget1->InitUnlockCard();
    UnlockCardWidget2->InitUnlockCard();
    UnlockCardWidget3->InitUnlockCard();
    UnlockCardWidget4->InitUnlockCard();
}

void UW_UnlockCardModeWidget::ChangeCardsLeft()
{
    ShopUnlockCardSubsystem->ChangeCardsLeft();
}

void UW_UnlockCardModeWidget::ChangeCardsRight()
{
    ShopUnlockCardSubsystem->ChangeCardsRight();
}