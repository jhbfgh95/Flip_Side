// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockCard/W_UnlockCardModeWidget.h"
#include "UI/ShopCard/W_ShopCardWidget.h"

#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"



#include "Components/Button.h"
#include "Components/UniformGridPanel.h"


void UW_UnlockCardModeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    ShopCardSubsystem->OnSelectCard.AddDynamic(this, &UW_UnlockCardModeWidget::SelectCard);
    ShopCardSubsystem->OnUnSelectCard.AddDynamic(this, &UW_UnlockCardModeWidget::UnSelectCard);

    ShopCardSubsystem->OnUnlockCard.AddDynamic(this, &UW_UnlockCardModeWidget::UnlockCardAdaptor);

    SelectPlayerCardButton->OnClicked.AddDynamic(this, &UW_UnlockCardModeWidget::SelectPlayerCard);
    UnSelectPlayerCardButton->OnClicked.AddDynamic(this,&UW_UnlockCardModeWidget::UnSelectPlayerCard);
    
    UnlockButton->OnClicked.AddDynamic(this ,&UW_UnlockCardModeWidget::UnlockCard);

    ////
    TArray<FCardData> CardData = ShopCardSubsystem->GetCardListArray();

    int32 CardListNum = CardData.Num();

    
    for (int32 i = 0; i < CardListNum; i++)
    {
        int32 Row = i / ColumnCount;
        int32 Col = i % ColumnCount;

        UW_ShopCardWidget* CardElement =Cast<UW_ShopCardWidget>(CreateWidget<UUserWidget>(GetWorld(), ShopCardWidget));

        if (CardElement && CardGrid)
        {
            CardElement->InitShopCard(CardData[i], UnlockSubsystem->IsCardUnlockByID(CardData[i].CardID));
            CardGrid->AddChildToUniformGrid(CardElement, Row, Col);
        }
    }
    UnSelectCard();
}   

void UW_UnlockCardModeWidget::NativeDestruct()
{
    Super::NativeDestruct();
}
void UW_UnlockCardModeWidget::SelectCard(FCardData SelectCardData)
{
    CurrentCardData = SelectCardData;
    SelectCardWidget->InitCard(SelectCardData);
    SelectCardWidget->SetVisibility(ESlateVisibility::Visible);
    UnlockButton->SetVisibility(ESlateVisibility::Visible);
    CancelImage->SetVisibility(ESlateVisibility::Visible);
    
    if(UnlockSubsystem->IsCardUnlockByID(CurrentCardData.CardID))
        SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);

    if(ShopCardSubsystem->CheckPlayerHaveCard(CurrentCardData.CardID))
         SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
}

void UW_UnlockCardModeWidget::UnSelectCard()
{
    SelectCardWidget->SetVisibility(ESlateVisibility::Hidden);
    UnlockButton->SetVisibility(ESlateVisibility::Hidden);
    CancelImage->SetVisibility(ESlateVisibility::Hidden);
    UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Hidden);
    SelectPlayerCardButton->SetVisibility(ESlateVisibility::Hidden);
}
    
void UW_UnlockCardModeWidget::UnlockCard()
{
    ShopCardSubsystem->UnlockCard();
}
	
void UW_UnlockCardModeWidget::ExcuteUnlock(int32 unlockCardId)
{
    UnSelectCard();
}
	
void UW_UnlockCardModeWidget::SelectPlayerCard()
{
    ShopCardSubsystem->SelectPlayerCard(CurrentCardData);

    SelectCardWidget->InitCard(CurrentCardData);
    SelectCardWidget->SetVisibility(ESlateVisibility::Visible);
    CancelImage->SetVisibility(ESlateVisibility::Visible);
    SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
    UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
}
	
void UW_UnlockCardModeWidget::UnSelectPlayerCard()
{
    ShopCardSubsystem->UnSelectPlayerCard(CurrentCardData);
}
	
void UW_UnlockCardModeWidget::UnlockCardAdaptor(int32 ID)
{
    SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
}