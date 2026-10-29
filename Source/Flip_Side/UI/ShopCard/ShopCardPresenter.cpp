// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/ShopCardPresenter.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"

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

    ShopCardMainWidget->GetShopCardWidget()->SetVisibility(ESlateVisibility::Collapsed);
}

void UShopCardPresenter::BuyCard(UW_ShopCardSlot* BuyCardSlot, int32 CardID)
{
    if(CardSubsystem->BuyCard(GetCardData(CardID)))
    {
        BuyCardSlot->SetCardSlotHidden();
		OnCardPurchased.Broadcast(CardID);
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
        AddSlot->OnDetailButtonHovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonHovered);
        AddSlot->OnDetailButtonUnhovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonUnhovered);
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

    ShopCardMainWidget
        ->GetShopPlayerCardSlotContainer()
        ->SetSelectCardCountText(CardSubsystem->GetPlayerSelectCardCount());

    // 카드를 실제로 골랐을 때만 소리를 냅니다. 이 프레젠터는 바깥 객체 없이 만들어져 GetWorld()가 비므로 위젯의 월드를 씁니다.
    if (UWorld* World = ShopCardMainWidget->GetWorld())
    {
        if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
        {
            SoundManager->PlayCardSelectSound();
        }
    }

	OnPlayerCardSelected.Broadcast(CardID);

}

void UShopCardPresenter::UnselectedPlayerSelectedCard(int32 RemoveSlotIndex)
{
    if(CardSubsystem->UnSelectPlayerCard(RemoveSlotIndex))
    {
        ShopCardMainWidget->GetShopPlayerCardSlotContainer()->RemovePlayerSelectCardSlot(RemoveSlotIndex);
        ShopCardMainWidget->GetShopPlayerCardSlotContainer()->SetSelectCardCountText(
            CardSubsystem->GetPlayerSelectCardCount());

        // 카드를 실제로 내려놓았을 때만 소리를 냅니다(목록에서 다시 누르기와 위쪽 선택 칸 누르기 모두 여기로 옵니다).
        if (UWorld* World = ShopCardMainWidget->GetWorld())
        {
            if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
            {
                SoundManager->PlayCardUnselectSound();
            }
        }
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



void UShopCardPresenter::HandleDetailButtonHovered()
{
    if (IsValid(ShopCardMainWidget))
    {
        if (UW_ShopCardWidget* CardWidget = ShopCardMainWidget->GetShopCardWidget(); IsValid(CardWidget))
        {
            CardWidget->SetDescriptionPage(1);
        }
    }
}

void UShopCardPresenter::HandleDetailButtonUnhovered()
{
    if (IsValid(ShopCardMainWidget))
    {
        if (UW_ShopCardWidget* CardWidget = ShopCardMainWidget->GetShopCardWidget(); IsValid(CardWidget))
        {
            CardWidget->SetDescriptionPage(0);
        }
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
        ShopCardSlotViews[i]->OnDetailButtonHovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonHovered);
        ShopCardSlotViews[i]->OnDetailButtonUnhovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonUnhovered);
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
    ShopCardMainWidget->GetShopPlayerCardSlotContainer()->SetSelectCardCountText(
        CardSubsystem->GetPlayerSelectCardCount());

    ShopPlayerCardSlotViews = ShopCardMainWidget->GetShopPlayerCardSlotContainer()->GetShopPlayerCardSlots();
    ShopPlayerSelectedCardSlotViews = ShopCardMainWidget->GetShopPlayerCardSlotContainer()->GetShopPlayerSelectedCardSlots();

    for(int i =0; i< ShopPlayerCardSlotViews.Num();i++)
    {
        ShopPlayerCardSlotViews[i]->OnSelectShopPlayerCard.AddDynamic(this, &UShopCardPresenter::SelectedPlayerCard);
        ShopPlayerCardSlotViews[i]->OnUnselectShopPlayerCard.AddDynamic(this, &UShopCardPresenter::UnselectedPlayerCard);
        ShopPlayerCardSlotViews[i]->OnHoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::HoveredCard);
        ShopPlayerCardSlotViews[i]->OnUnhoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::UnhoveredCard);
        ShopPlayerCardSlotViews[i]->OnDetailButtonHovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonHovered);
        ShopPlayerCardSlotViews[i]->OnDetailButtonUnhovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonUnhovered);
    }
    for(int i =0; i<ShopPlayerSelectedCardSlotViews.Num();i++)
    {
        ShopPlayerSelectedCardSlotViews[i]->OnUnselectedPlayerCard.AddDynamic(this,  &UShopCardPresenter::UnselectedPlayerSelectedCard);
        ShopPlayerSelectedCardSlotViews[i]->OnHoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::HoveredCard);
        ShopPlayerSelectedCardSlotViews[i]->OnUnhoveredShopCardSlot.AddDynamic(this, &UShopCardPresenter::UnhoveredCard);
        ShopPlayerSelectedCardSlotViews[i]->OnDetailButtonHovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonHovered);
        ShopPlayerSelectedCardSlotViews[i]->OnDetailButtonUnhovered.AddUniqueDynamic(this, &ThisClass::HandleDetailButtonUnhovered);
    }

}

FCardData UShopCardPresenter::GetCardData(int32 GetCardID)
{
    FCardData ReturnCardData;
    ReturnCardData.CardID = -1;
    DataManager->TryGetCard(GetCardID, ReturnCardData);
    return ReturnCardData;
}
