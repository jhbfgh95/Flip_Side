#define PLAYER_SELECTCARD_MAX 3


#include "UI/ShopCard/W_ShopPlayerCardSlotContainer.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/ShopCard/W_ShopPlayerCardSlot.h"
#include "DataTypes/CardTypes.h"
#include "UI/ShopCard/W_ShopPlayerSelectedCardSlot.h"


void UW_ShopPlayerCardSlotContainer::InitWidget(const TArray<FCardData> UnlockCardData)
{
    for(int i =0; i< UnlockCardData.Num(); i++)
    {
        AddPlayerCardSlot(UnlockCardData[i]);
    }

    UsingSelectSlotCount = -1;
    FCardData DefaultsCardData;
    DefaultsCardData.CardID = -1;

    for(int i =0; i<PLAYER_SELECTCARD_MAX; i++)
    {
        UW_ShopPlayerSelectedCardSlot* CardSlotWidget =Cast<UW_ShopPlayerSelectedCardSlot>(CreateWidget<UUserWidget>(GetWorld(), PlayerSelectedCardSlotWidget));
        if (CardSlotWidget)
        {
            SelectedPlayerCardSlots.Add(CardSlotWidget);
            UVerticalBoxSlot* VSlot = PlayerSelectSlotBox->AddChildToVerticalBox(CardSlotWidget);
            if(VSlot)
                VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

            CardSlotWidget->SetVisibility(ESlateVisibility::Collapsed);
            CardSlotWidget->InitCardSlot(i);
        }
    }
}

UW_ShopPlayerCardSlot* UW_ShopPlayerCardSlotContainer::AddPlayerCardSlot(FCardData AddCardData)
{
    UW_ShopPlayerCardSlot* CardSlotWidget = Cast<UW_ShopPlayerCardSlot>(CreateWidget<UUserWidget>(GetWorld(), PlayerCardSlotWidget));

    if (CardSlotWidget)
    {
        PlayerCardSlots.Add(CardSlotWidget);
        UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(CardSlotWidget);
        if(VSlot)
            VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

        CardSlotWidget->InitCardSlot(AddCardData);
    }

    return CardSlotWidget;
}

void UW_ShopPlayerCardSlotContainer::AddPlayerSelectCardSlot(FCardData AddCardData, UW_ShopPlayerCardSlot* ConnectedSlot)
{
    for(int i =0; i< PlayerCardSlots.Num();i++)
    {
        if(PlayerCardSlots[i]->GetSlotCardID()==AddCardData.CardID)
        {
            if(SelectedPlayerCardSlots.IsValidIndex(UsingSelectSlotCount+1))
            {
                UsingSelectSlotCount++;
                SelectedPlayerCardSlots[UsingSelectSlotCount]->SetCardSlot(AddCardData, ConnectedSlot);
                SelectedPlayerCardSlots[UsingSelectSlotCount]->SetVisibility(ESlateVisibility::Visible);
                
            }
            return;
        }
    }
}

void UW_ShopPlayerCardSlotContainer::RemovePlayerSelectCardSlot(int32 RemoveIndex)
{
    if (RemoveIndex < 0 || RemoveIndex > UsingSelectSlotCount)
        return;

    for (int32 i = RemoveIndex; i < UsingSelectSlotCount; ++i)
    {
        const FCardData& NextCardData = SelectedPlayerCardSlots[i + 1]->GetCardData();

        UW_ShopPlayerCardSlot* NextConnectedSlot = SelectedPlayerCardSlots[i + 1]->GetConnnectedSlot();

        SelectedPlayerCardSlots[i]->SetCardSlot(NextCardData,NextConnectedSlot);
    }

    SelectedPlayerCardSlots[UsingSelectSlotCount]->ClearSlot();
    UsingSelectSlotCount--;
}


TArray<UW_ShopPlayerCardSlot*> UW_ShopPlayerCardSlotContainer::GetShopPlayerCardSlots()
{
    return PlayerCardSlots;
}
	
TArray<UW_ShopPlayerSelectedCardSlot*> UW_ShopPlayerCardSlotContainer::GetShopPlayerSelectedCardSlots()
{
    return SelectedPlayerCardSlots;
}
	
int32 UW_ShopPlayerCardSlotContainer::GetPlayerSelectedCardIndex(int32 CardID)
{
    for(int i =0; i<SelectedPlayerCardSlots.Num(); i++)
    {  
        if(SelectedPlayerCardSlots[i]->GetCardData().CardID == -1)
            continue;
        if(SelectedPlayerCardSlots[i]->GetCardData().CardID == CardID)
            return i;
    }
    return -1;
}