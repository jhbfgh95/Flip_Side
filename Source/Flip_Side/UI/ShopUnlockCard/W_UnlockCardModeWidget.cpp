// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockCard/W_UnlockCardModeWidget.h"
#include "UI/ShopUnlockCard/W_UnlockCardWidget.h"

#include "Subsystem/ShopLevel/ShopUnlockCardWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
void UW_UnlockCardModeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopUnlockCardSubsystem = GetWorld()->GetSubsystem<UShopUnlockCardWSubsystem>();
    DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    ShopUnlockCardSubsystem->OnSelectUnlockCard.AddDynamic(this, &UW_UnlockCardModeWidget::SelectCard);
    ShopUnlockCardSubsystem->OnUnSelectUnlockCard.AddDynamic(this, &UW_UnlockCardModeWidget::UnSelectCard);

    ShopUnlockCardSubsystem->OnUnlockSelectCard.AddDynamic(this, &UW_UnlockCardModeWidget::ExcuteUnlock);

    
    UnlockButton->OnClicked.AddDynamic(this ,&UW_UnlockCardModeWidget::UnlockCard);

    UnlockButton->SetVisibility(ESlateVisibility::Hidden);
    SelectUnlockCardWidget->SetVisibility(ESlateVisibility::Hidden);

    ////
    TArray<FCardData> CardData = ShopUnlockCardSubsystem->GetCardListArray();

    int32 CardListNum = CardData.Num();

    
    for (int32 i = 0; i < CardListNum; i++)
    {
        int32 Row = i / ColumnCount;
        int32 Col = i % ColumnCount;

        UW_UnlockCardWidget* CardElement =Cast<UW_UnlockCardWidget>(CreateWidget<UUserWidget>(GetWorld(), UnlockCardWidget));

        if (CardElement && CardGrid)
        {
            CardElement->InitUnlockCard(CardData[i]);
            CardGrid->AddChildToUniformGrid(CardElement, Row, Col);
        }
    }

}   

void UW_UnlockCardModeWidget::NativeDestruct()
{
    Super::NativeDestruct();
}
void UW_UnlockCardModeWidget::ChangeCardsLeft()
{
    ShopUnlockCardSubsystem->ChangeCardsLeft();
}

void UW_UnlockCardModeWidget::ChangeCardsRight()
{
    ShopUnlockCardSubsystem->ChangeCardsRight();
}
	
void UW_UnlockCardModeWidget::SelectCard(int32 SelectCardDataID)
{
    FCardData CardData;
    DataSubsystem->TryGetCard(SelectCardDataID, CardData);
    SelectUnlockCardWidget->InitCard(CardData);
    SelectUnlockCardWidget->SetVisibility(ESlateVisibility::Visible);
    UnlockButton->SetVisibility(ESlateVisibility::Visible);
}

void UW_UnlockCardModeWidget::UnSelectCard()
{
    SelectUnlockCardWidget->SetVisibility(ESlateVisibility::Hidden);
    UnlockButton->SetVisibility(ESlateVisibility::Hidden);
}
    
void UW_UnlockCardModeWidget::UnlockCard()
{
    ShopUnlockCardSubsystem->UnlockCard();
}
	
void UW_UnlockCardModeWidget::ExcuteUnlock(int32 unlockCardId)
{
    UnSelectCard();
}