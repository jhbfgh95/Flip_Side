// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "SubSystem/UnlockGISubsystem.h"
bool UShopCardWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel")) || MapName.Contains(TEXT("L_Tutorial_Shop_Level"));
} 

void UShopCardWSubsystem::OnWorldBeginPlay(UWorld& World)
{
    Super::OnWorldBeginPlay(World);

    DM = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    UnlockSubsystem->OnUnlockCard.AddDynamic(this,&UShopCardWSubsystem::AddCardListToUnlockCard);

    DM->TryGetAllCards(TotalCardList);

    DefaultCard.CardID =-1;
    
    for(int i =0; i<3; i++)
        PlayerCardList.Add(DefaultCard);

}

void UShopCardWSubsystem::Deinitialize()
{
    UnlockSubsystem->OnUnlockCard.RemoveAll(this);
    Super::Deinitialize();
}

void UShopCardWSubsystem::SelectCard(FCardData CardData)
{
    /*
    if(UnlockCardList.Num()<=0)
        return;
        
    if(!CanSelectCard())
        return;
        
    for(int i =0; i< PlayerCardList.Num(); i++)
    {
        if(PlayerCardList[i].CardID ==-1)
        {
            
            PlayerCardList[i] = UnlockCardList[CurrentCardListNum];
            OnCardSelected.Broadcast(i, PlayerCardList[i]);
            return;
        }
    }*/
    
    CurrentSelectCard = CardData;
    OnSelectCard.Broadcast(CurrentSelectCard);
}
	
void UShopCardWSubsystem::UnSelectCard()
{
    CurrentSelectCard = DefaultCard;
    OnUnSelectCard.Broadcast();
}

void UShopCardWSubsystem::UnlockCard()
{
    if(!UnlockSubsystem->IsCardUnlockByID(CurrentSelectCard.CardID))
    {
        UnlockSubsystem->UnlockCard(CurrentSelectCard.CardID);
        OnUnlockCard.Broadcast(CurrentSelectCard.CardID);
    }
}

bool UShopCardWSubsystem::CanSelectCard()
{
    for(int i =0; i<PlayerCardList.Num(); i++)
    {
        if(PlayerCardList[i].CardID == CurrentSelectCard.CardID)
        {
            return false;
        }
    }

    for(int i =0; i< PlayerCardList.Num(); i++)
    {
        if(PlayerCardList[i].CardID ==-1)
        {
            return true;
        }
    }
    return false;
}

int32 UShopCardWSubsystem::CanSelectPlayerIndex()
{
    for(int i =0; i<PlayerCardList.Num(); i++)
    {
        if(PlayerCardList[i].CardID == -1)
        {
            return i;
        }
    }
    return -1;
}

int32 UShopCardWSubsystem::GetPlayerCardID(int32 index)
{
    if(index<PlayerCardList.Num())
        return PlayerCardList[index].CardID;
    else 
        return -1;
}

void UShopCardWSubsystem::SetUnlockCard()
{
    for(int i =0; i<UnlockSubsystem->GetUnlockCardArrayNum();i++)
    {
        if(DM)
        {
            int32 GetCardID = UnlockSubsystem->GetUnlockCardID(i);
            FCardData CardData;

            if(DM->TryGetCard(GetCardID,CardData))
            {
                UnlockCardList.Add(CardData);
            }
            
        }
    }
}

void UShopCardWSubsystem::AddCardListToUnlockCard(int32 UnlockCardID)
{
    if(DM)
    {
        FCardData CardData;
        if(DM->TryGetCard(UnlockCardID,CardData))
        {
            UnlockCardList.Add(CardData);
        }
        
    }
}
	
	
void UShopCardWSubsystem::SelectPlayerCard(FCardData CardData)
{
    if(CanSelectCard())
    {   
        int SelectIndex = CanSelectPlayerIndex();
        PlayerCardList[SelectIndex] = (CurrentSelectCard);
        OnChangePlayerCard.Broadcast();
        OnSelectPlayerCard.Broadcast(CardData);
    }
}
	
void UShopCardWSubsystem::UnSelectPlayerCard(FCardData CardData)
{
    for(int i =0; i< PlayerCardList.Num();i++)
    {
        if(CardData.CardID == PlayerCardList[i].CardID)
        {
            PlayerCardList[i] = DefaultCard;
            OnChangePlayerCard.Broadcast();
            return;
        }
    }
}

TArray<FCardData> UShopCardWSubsystem::GetCardListArray()
{
    return TotalCardList;
}


TArray<FCardData> UShopCardWSubsystem::GetPlayerCardList()
{
    return PlayerCardList;
}
	
bool UShopCardWSubsystem::CheckPlayerHaveCard(int32 CardID)
{
    for(int i =0; i< PlayerCardList.Num();i++)
    {
        if(PlayerCardList[i].CardID == CardID)
            return true;
    }
    return false;
}