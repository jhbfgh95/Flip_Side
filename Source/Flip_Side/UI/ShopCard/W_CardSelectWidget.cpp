// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_CardSelectWidget.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Components/Button.h"
#include "DataTypes/CardTypes.h"
#include "UI/W_CardWidget.h"
void UW_CardSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    ShopCardSubSystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    RightCardButton->OnClicked.AddDynamic(this, &UW_CardSelectWidget::ClickRightCardButton);
    LeftCardButton->OnClicked.AddDynamic(this, &UW_CardSelectWidget::ClickLeftCardButton);
    SelectCardButton->OnClicked.AddDynamic(this, &UW_CardSelectWidget::ClickSelectCardButton);
    ShopCardSubSystem->OnCardChanged.AddDynamic(this, &UW_CardSelectWidget::InitCard);

    UnlockSubsystem->OnUnlockCard.AddDynamic(this, &UW_CardSelectWidget::SetShowCard);
    SetShowCard(0);
}

void UW_CardSelectWidget::NativeDestruct() 
{
    UnlockSubsystem->OnUnlockCard.RemoveAll(this);
    Super::NativeDestruct();
}


void UW_CardSelectWidget::ClickLeftCardButton()
{
    ShopCardSubSystem->SetPreviousCard();
}

void UW_CardSelectWidget::ClickRightCardButton()
{
    ShopCardSubSystem->SetNextCard();
}


void UW_CardSelectWidget::ClickSelectCardButton()
{
    ShopCardSubSystem->SelectCard();
}


void UW_CardSelectWidget::InitCard(FCardData CardData)
{
    CurrentCardWidget->InitCard(CardData);
}

void UW_CardSelectWidget::SetShowCard(int32 UnlockCardID)
{
    if(ShopCardSubSystem->GetCurrentCard().CardID == -1)
    {
        CurrentCardWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        CurrentCardWidget->SetVisibility(ESlateVisibility::Visible);
        CurrentCardWidget->InitCard(ShopCardSubSystem->GetCurrentCard());
    }
}