// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CardSelectWidget.h"
#include "Subsystem/ShopCardWSubsystem.h"
#include "Components/Button.h"
#include "DataTypes/CardTypes.h"
#include "UI/W_CardWidget.h"
void UW_CardSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    ShopCardSubSystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();

    RightCardButton->OnClicked.AddDynamic(this, &UW_CardSelectWidget::ClickRightCardButton);
    LeftCardButton->OnClicked.AddDynamic(this, &UW_CardSelectWidget::ClickLeftCardButton);
    SelectCardButton->OnClicked.AddDynamic(this, &UW_CardSelectWidget::ClickSelectCardButton);
    ShopCardSubSystem->OnCardChanged.AddDynamic(this, &UW_CardSelectWidget::InitCard);

    
    CurrentCardWidget->InitCard(ShopCardSubSystem->GetCurrentCard());
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