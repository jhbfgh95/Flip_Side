// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/ShopCardPresenter.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"

#include "UI/ShopCard/W_ShopCardMainWidget.h"
#include "UI/ShopCard/W_ShopCardSlotContainer.h"
#include "UI/ShopCard/W_ShopCardSlot.h"
#include "UI/ShopCard/W_ShopPlayerCardSlotContainer.h"
#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "UI/ShopCard/W_ShopPlayerSelectedCardSlot.h"
#include "UI/ShopCard/W_ShopCardWidget.h"

void UShopCardPresenter::InitPresenter(UW_ShopCardMainWidget* InShopCardMainWidget, 
    UShopCardWSubsystem* InShopCardSubsystem, UDataManagerSubsystem* InDataManager
    ,UUnlockGISubsystem* InUnlockSubsystem)
{
    ShopCardMainWidget =InShopCardMainWidget;
    CardSubsystem =InShopCardSubsystem;
    DataManager =InDataManager;
    UnlockSubsystem = InUnlockSubsystem;

    UnlockSubsystem->OnUnlockCard.AddDynamic(this, &UShopCardPresenter::AddCard);

    InitShopCardSlot();
    InitShopPlayerCardSlot();
}

void UShopCardPresenter::BuyCard(UW_ShopCardSlot* BuyCardSlot, int32 CardID)
{
    if(CardSubsystem->BuyCard(GetCardData(CardID)))
    {
        BuyCardSlot->SetCardSlotHidden();
    }
}
	
void UShopCardPresenter::AddCard(int32 CardID)
{
    FCardData InCardData = GetCardData(CardID);
    UW_ShopPlayerCardSlot* AddSlot = ShopCardMainWidget->GetShopPlayerCardSlotContainer()->AddPlayerCardSlot(InCardData);
    if(AddSlot)
    {
        AddSlot->OnSelectShopPlayerCard.AddDynamic(this, &UShopCardPresenter::SelectedPlayerCard);
        AddSlot->OnUnselectShopPlayerCard.AddDynamic(this, &UShopCardPresenter::UnselectedPlayerCard);
        AddSlot->OnHoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::HoveredCard);
        AddSlot->OnUnhoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::UnhoveredCard);
        ShopPlayerCardSlotViews.Add(AddSlot);
    }
    
}

void UShopCardPresenter::SelectedPlayerCard(UW_ShopPlayerCardSlot* SelectedCardSlot, int32 CardID)
{
    const FCardData CardData = GetCardData(CardID);
    const int32 AddIndex = CardSubsystem->SelectPlayerCard(CardData);

    if (AddIndex == -1)
    {
        return;
    }

    SelectedCardSlot->SetSlotIsSelected(true);

    ShopCardMainWidget
        ->GetShopPlayerCardSlotContainer()
        ->AddPlayerSelectCardSlot(CardData, SelectedCardSlot);

}

void UShopCardPresenter::UnselectedPlayerSelectedCard(int32 RemoveSlotIndex)
{
    if(CardSubsystem->UnSelectPlayerCard(RemoveSlotIndex))
    {
        ShopCardMainWidget->GetShopPlayerCardSlotContainer()->RemovePlayerSelectCardSlot(RemoveSlotIndex);
    }
}
void UShopCardPresenter::UnselectedPlayerCard(int32 CardID)
{
    int32 SelectedSlotIndex = ShopCardMainWidget->GetShopPlayerCardSlotContainer()->GetPlayerSelectedCardIndex(CardID);
    UnselectedPlayerSelectedCard(SelectedSlotIndex);
}


void UShopCardPresenter::HoveredCard(int32 CardID)
{
	CurrentHoveredCardID = CardID;
	if(UW_ShopCardWidget* CardWidget = ShopCardMainWidget->GetShopCardWidget())
	{
		CardWidget->SetCardData(GetCardData(CurrentHoveredCardID));
	}
}
void UShopCardPresenter::UnhoveredCard()
{
	CurrentHoveredCardID = -1;
	if(UW_ShopCardWidget* CardWidget = ShopCardMainWidget->GetShopCardWidget())
	{
		CardWidget->UnsetCardData();
	}
}



void UShopCardPresenter::InitShopCardSlot()
{
    TArray<FCardData> ShopCardData = CardSubsystem->GetCardListArray();
    ShopCardMainWidget->GetShopCardSlotContainer()->InitWidget(ShopCardData);
    ShopCardSlotViews = ShopCardMainWidget->GetShopCardSlotContainer()->GetShopCardSlots();

    for(int i =0; i< ShopCardSlotViews.Num(); i++)
    {   
        ShopCardSlotViews[i]->OnBuyCard.AddDynamic(this, &UShopCardPresenter::BuyCard);
        ShopCardSlotViews[i]->OnHoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::HoveredCard);
        ShopCardSlotViews[i]->OnUnhoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::UnhoveredCard);
    }
}

void UShopCardPresenter::InitShopPlayerCardSlot()
{
    TArray<int32> UnlockCardID = UnlockSubsystem->GetUnlockCardArray();
    TArray<FCardData> InCardDataArray;
    FCardData InCardData;
    for(int i = 0;i < UnlockCardID.Num(); i++)
    {
        InCardData = GetCardData(UnlockCardID[i]);
        InCardDataArray.Add(InCardData);
    }
    ShopCardMainWidget->GetShopPlayerCardSlotContainer()->InitWidget(InCardDataArray);

    ShopPlayerCardSlotViews = ShopCardMainWidget->GetShopPlayerCardSlotContainer()->GetShopPlayerCardSlots();
    ShopPlayerSelectedCardSlotViews = ShopCardMainWidget->GetShopPlayerCardSlotContainer()->GetShopPlayerSelectedCardSlots();

    for(int i =0; i< ShopPlayerCardSlotViews.Num();i++)
    {
        ShopPlayerCardSlotViews[i]->OnSelectShopPlayerCard.AddDynamic(this, &UShopCardPresenter::SelectedPlayerCard);
        ShopPlayerCardSlotViews[i]->OnUnselectShopPlayerCard.AddDynamic(this, &UShopCardPresenter::UnselectedPlayerCard);
        ShopPlayerCardSlotViews[i]->OnHoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::HoveredCard);
        ShopPlayerCardSlotViews[i]->OnUnhoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::UnhoveredCard);
    }
    for(int i =0; i<ShopPlayerSelectedCardSlotViews.Num();i++)
    {
        ShopPlayerSelectedCardSlotViews[i]->OnUnselectedPlayerCard.AddDynamic(this,  &UShopCardPresenter::UnselectedPlayerSelectedCard);
        ShopPlayerSelectedCardSlotViews[i]->OnHoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::HoveredCard);
        ShopPlayerSelectedCardSlotViews[i]->OnUnhoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::UnhoveredCard);
    }

}

FCardData UShopCardPresenter::GetCardData(int32 GetCardID)
{
    FCardData ReturnCardData;
    ReturnCardData.CardID = -1;
    DataManager->TryGetCard(GetCardID, ReturnCardData);
    return ReturnCardData;
}
