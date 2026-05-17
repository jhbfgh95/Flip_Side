// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockCard/W_UnlockCardModeWidget.h"
#include "UI/ShopCard/W_ShopCardWidget.h"

#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/W_PriceWidget.h"


#include "Components/Button.h"
#include "Components/UniformGridPanel.h"


void UW_UnlockCardModeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    ShopCardSubsystem->OnSelectCard.AddDynamic(this, &UW_UnlockCardModeWidget::SetSelectShopCardPopup);
    ShopCardSubsystem->OnUnSelectCard.AddDynamic(this, &UW_UnlockCardModeWidget::CloseSelectCardPopup);

    ShopCardSubsystem->OnSelectPlayerCard.AddDynamic(this, &UW_UnlockCardModeWidget::SetSelectPlayerCardPopup);
    ShopCardSubsystem->OnUnSelectPlayerCard.AddDynamic(this, &UW_UnlockCardModeWidget::SetUnSelectPlayerCardPopup);
    
    ShopCardSubsystem->OnUnlockCard.AddDynamic(this, &UW_UnlockCardModeWidget::UnlockCardAdaptor);

    SelectPlayerCardButton->OnClicked.AddDynamic(this, &UW_UnlockCardModeWidget::SelectPlayerCard);
    UnSelectPlayerCardButton->OnClicked.AddDynamic(this,&UW_UnlockCardModeWidget::UnSelectPlayerCard);
    

    UnlockButton->OnClicked.AddDynamic(this ,&UW_UnlockCardModeWidget::UnlockCard);
    CancelButton->OnClicked.AddDynamic(this ,&UW_UnlockCardModeWidget::HandleCancelButton);
    ////
    TArray<FCardData> CardData = ShopCardSubsystem->GetCardListArray();

    //더 많은 황금 제거
    if(CardData.IsValidIndex(4))
    {
        CardData.RemoveAt(3);
    }
      
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
    CloseSelectCardPopup();
}   

void UW_UnlockCardModeWidget::NativeDestruct()
{
    ShopCardSubsystem->OnSelectCard.RemoveAll(this);
    ShopCardSubsystem->OnSelectCard.RemoveAll(this);

    ShopCardSubsystem->OnSelectCard.RemoveAll(this);
    ShopCardSubsystem->OnSelectCard.RemoveAll(this);
    
    ShopCardSubsystem->OnUnlockCard.RemoveAll(this);
    Super::NativeDestruct();
}



void UW_UnlockCardModeWidget::SetSelectShopCardPopup(FCardData SelectCardData)
{
    CurrentCardData = SelectCardData;
    SelectCardWidget->InitCard(SelectCardData);
    
    SelectCardWidget->SetVisibility(ESlateVisibility::Visible);
    CancelImage->SetVisibility(ESlateVisibility::Visible);
    CancelButton->SetVisibility(ESlateVisibility::Visible);


    UnlockButton->SetVisibility(ESlateVisibility::Hidden);
    CardPriceText->SetVisibility(ESlateVisibility::Hidden);
    SelectPlayerCardButton->SetVisibility(ESlateVisibility::Hidden);
    UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Hidden);

    //잠겨있는지 검사
    if(!UnlockSubsystem->IsCardUnlockByID(CurrentCardData.CardID))
    {
        UnlockButton->SetVisibility(ESlateVisibility::Visible);
        
        CardPriceText->SetVisibility(ESlateVisibility::Visible);
        CardPriceText->SetPriceText(SelectCardData.Price);
    }
    else
    {
        SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
        UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
    }
}


void UW_UnlockCardModeWidget::SetSelectPlayerCardPopup(FCardData SelectCardData)
{
    CurrentCardData = SelectCardData;
    SelectCardWidget->InitCard(SelectCardData);

    SelectCardWidget->SetVisibility(ESlateVisibility::Visible);
    CancelImage->SetVisibility(ESlateVisibility::Visible);
    CancelButton->SetVisibility(ESlateVisibility::Visible);
    
    UnlockButton->SetVisibility(ESlateVisibility::Hidden);
    CardPriceText->SetVisibility(ESlateVisibility::Hidden);
    SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
    UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
    /*
    if(ShopCardSubsystem->CheckPlayerHaveCard(CurrentCardData.CardID))
    {
        UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
    }*/
}

void UW_UnlockCardModeWidget::SetUnSelectPlayerCardPopup(FCardData UnSelectCardData, int32 PlayerCardIndex)
{
    if(SelectCardWidget->GetVisibility() != ESlateVisibility::Visible)
        return;
    if(UnSelectCardData.CardID != CurrentCardData.CardID)
        return;
    UnlockButton->SetVisibility(ESlateVisibility::Hidden);
    CardPriceText->SetVisibility(ESlateVisibility::Hidden);
    SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
    UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
}


void UW_UnlockCardModeWidget::SelectCard(FCardData SelectCardData)
{
    CurrentCardData = SelectCardData;
    SelectCardWidget->InitCard(SelectCardData);

    SelectCardWidget->SetVisibility(ESlateVisibility::Visible);
    CancelImage->SetVisibility(ESlateVisibility::Visible);
    CancelButton->SetVisibility(ESlateVisibility::Visible);

    //1. 카드 해금 됬는지
    if(UnlockSubsystem->IsCardUnlockByID(CurrentCardData.CardID))
    {
        //2. 카드를 가지고 있는지
        if(ShopCardSubsystem->CheckPlayerHaveCard(CurrentCardData.CardID))
        {
            UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        UnlockButton->SetVisibility(ESlateVisibility::Visible);
        CardPriceText->SetVisibility(ESlateVisibility::Visible);
    }
}

void UW_UnlockCardModeWidget::CloseSelectCardPopup()
{
    SelectCardWidget->SetVisibility(ESlateVisibility::Hidden);
    UnlockButton->SetVisibility(ESlateVisibility::Hidden);
    CardPriceText->SetVisibility(ESlateVisibility::Hidden);
    CancelImage->SetVisibility(ESlateVisibility::Hidden);
    UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Hidden);
    SelectPlayerCardButton->SetVisibility(ESlateVisibility::Hidden);
    CancelButton->SetVisibility(ESlateVisibility::Hidden);
}

void UW_UnlockCardModeWidget::SetPlayerCard()
{
    ShopCardSubsystem->SelectPlayerCard(CurrentCardData);
}

void UW_UnlockCardModeWidget::UnlockCard()
{
    ShopCardSubsystem->UnlockCard();
}
	
	
void UW_UnlockCardModeWidget::SelectPlayerCard()
{
    ShopCardSubsystem->SelectPlayerCard(CurrentCardData);
}
	
void UW_UnlockCardModeWidget::UnSelectPlayerCard()
{
    ShopCardSubsystem->UnSelectPlayerCard(CurrentCardData);
}
	
void UW_UnlockCardModeWidget::UnlockCardAdaptor(int32 ID)
{
    if(ID == CurrentCardData.CardID)
    {
        UnlockButton->SetVisibility(ESlateVisibility::Hidden);
        CardPriceText->SetVisibility(ESlateVisibility::Hidden);
        
        SelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
        UnSelectPlayerCardButton->SetVisibility(ESlateVisibility::Visible);
    }
}

void UW_UnlockCardModeWidget::HandleCancelButton()
{
    ShopCardSubsystem->UnSelectCard();
}