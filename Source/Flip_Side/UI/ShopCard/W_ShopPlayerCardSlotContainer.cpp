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


void UW_ShopPlayerCardSlotContainer::NativeConstruct()
{
    Super::NativeConstruct();
    
    CardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    UnlockSubsystem->OnUnlockCard.AddDynamic(this, &UW_ShopPlayerCardSlotContainer::UpdatePlayerCard);
    
    CardSubsystem->OnSelectPlayerCard.AddDynamic(this, &UW_ShopPlayerCardSlotContainer::AddPlayerSelectCardSlot);
    CardSubsystem->OnUnselectPlayerCard.AddDynamic(this, &UW_ShopPlayerCardSlotContainer::UpdatePlayerSelectCard);



    const TArray<int32> UnlockCardID = UnlockSubsystem->GetUnlockCardArray();

    for(int i =0; i<UnlockCardID.Num(); i++)
    {
        FCardData SetCardData;
        DataManager->TryGetCard(UnlockCardID[i], SetCardData);

        AddPlayerCardSlot(SetCardData);
    }
    FCardData DefaultsCardData;
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
            CardSlotWidget->InitCardSlot(CardSubsystem);
        }
    }
}

void UW_ShopPlayerCardSlotContainer::NativeDestruct()
{
    
    UnlockSubsystem->OnUnlockCard.RemoveAll(this);
    CardSubsystem->OnSelectPlayerCard.RemoveAll(this);
    CardSubsystem->OnUnselectPlayerCard.RemoveAll(this);

    Super::NativeDestruct();
}


void UW_ShopPlayerCardSlotContainer::UpdatePlayerCard(int32 CardID)
{
    FCardData UpdateCardData;
    if(DataManager->TryGetCard(CardID, UpdateCardData))
    {
        AddPlayerCardSlot(UpdateCardData);
    }

}

void UW_ShopPlayerCardSlotContainer::UpdatePlayerSelectCard()
{
    UsingSelectSlotCount = 0;
    for(int i =0; i <SelectedPlayerCardSlots.Num();i++)
    {
        SelectedPlayerCardSlots[i]->SetVisibility(ESlateVisibility::Collapsed);
    }

    const TArray<FCardData> PlayerCards = CardSubsystem->GetPlayerCardList();
    for(int i =0; i <PlayerCards.Num();i++)
    {
        AddPlayerSelectCardSlot(PlayerCards[i]);
    }
}

void UW_ShopPlayerCardSlotContainer::AddPlayerCardSlot(FCardData AddCardData)
{
    UW_ShopPlayerCardSlot* CardSlotWidget = Cast<UW_ShopPlayerCardSlot>(CreateWidget<UUserWidget>(GetWorld(), PlayerCardSlotWidget));

    if (CardSlotWidget)
    {
        PlayerCardSlots.Add(CardSlotWidget);
        UVerticalBoxSlot* VSlot = SlotBox->AddChildToVerticalBox(CardSlotWidget);
        if(VSlot)
            VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

        CardSlotWidget->InitCardSlot(AddCardData, CardSubsystem);
    }
}

void UW_ShopPlayerCardSlotContainer::AddPlayerSelectCardSlot(FCardData AddCardData)
{
    UE_LOG(LogTemp,Warning, TEXT("델리게이트입력받았음"));

    UW_ShopPlayerCardSlot* ConnectedSlot;
    for(int i =0; i< PlayerCardSlots.Num();i++)
    {
        if(PlayerCardSlots[i]->GetSlotCardID()==AddCardData.CardID)
        {
            ConnectedSlot = PlayerCardSlots[i];
            if(SelectedPlayerCardSlots.IsValidIndex(UsingSelectSlotCount))
            {
                SelectedPlayerCardSlots[UsingSelectSlotCount]->SetCardSlot(AddCardData, ConnectedSlot);
                SelectedPlayerCardSlots[UsingSelectSlotCount]->SetVisibility(ESlateVisibility::Visible);
                UsingSelectSlotCount++;
            }
            return;
        }
    }

    
}
