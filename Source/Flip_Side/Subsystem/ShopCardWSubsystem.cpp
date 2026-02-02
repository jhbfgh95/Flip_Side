// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopCardWSubsystem.h"

bool UShopCardWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel"));
} 

void UShopCardWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FCardData testCard;
    
    for(int i =0; i<6; i++)
    {
        testCard.CardID = i;
        testCard.CardName = FString::FromInt(i);
        testCard.Card_Description= FString::FromInt(i);
        CardList.Add(testCard);
    }
    for(int i =0; i<3; i++)
    {
        testCard.CardID = -1;
        PlayerCardList.Add(testCard);
    }
    OnCardChanged.Broadcast(CardList[CurrentCardListNum]);
}


void UShopCardWSubsystem::SetNextCard()
{
    //다음 버튼을 눌렀을 때 끝인지 검사
    if(CardList.Num() <= CurrentCardListNum+1 )
    {
        UE_LOG(LogTemp, Warning, TEXT("끝에 도달함"));
        return;
    }
    CurrentCardListNum++;
    OnCardChanged.Broadcast(CardList[CurrentCardListNum]);
}

void UShopCardWSubsystem::SetPreviousCard()
{
    if(CurrentCardListNum-1 < 0 )
    {
        UE_LOG(LogTemp, Warning, TEXT("끝에 도달함"));
        return;
    }

    CurrentCardListNum--;
    OnCardChanged.Broadcast(CardList[CurrentCardListNum]);
    UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentCardListNum);
}

void UShopCardWSubsystem::SelectCard()
{
    if(!CanSelectCard())
        return;
        
    for(int i =0; i< PlayerCardList.Num(); i++)
    {
        if(PlayerCardList[i].CardID ==-1)
        {
            PlayerCardList[i] = CardList[CurrentCardListNum];
            OnCardSelected.Broadcast(i, PlayerCardList[i]);
            return;
        }
    }
    
}

bool UShopCardWSubsystem::CanSelectCard()
{
    for(int i =0; i<PlayerCardList.Num(); i++)
    {
        if(PlayerCardList[i].CardID == CardList[CurrentCardListNum].CardID)
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

FCardData UShopCardWSubsystem::GetCurrentCard()
{
    return CardList[CurrentCardListNum];
}


void UShopCardWSubsystem::RemoveHandCard(int32 HandIndex)
{
    if(HandIndex<PlayerCardList.Num())
    {
        PlayerCardList[HandIndex].CardID = -1;
    }
}