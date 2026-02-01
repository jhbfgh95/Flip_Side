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
        testCard.CardName = FString::FromInt(i);
        testCard.Card_Description= FString::FromInt(i);
        CardList.Add(testCard);
    }
    for(int i =0; i<3; i++)
    {
        testCard.CardID = -1;
        PlayerCardList.Add(testCard);
    }
}


void UShopCardWSubsystem::SetNextCard()
{
    if(CurrentCardListNum < CardList.Num()-1)
    {
        CurrentCardListNum++;
        //카드 중복 체크
        for(int i =0; i < PlayerCardList.Num();i++)
        {
            if(CurrentCardListNum+1 < PlayerCardList.Num())
            {
                if(PlayerCardList[i].CardID == CardList[CurrentCardListNum].CardID)
                {
                    UE_LOG(LogTemp, Warning, TEXT("카드 일차 %d"), i);
                    CurrentCardListNum++;
                }
            }
            
        }
    }
    OnCardChanged.Broadcast(CardList[CurrentCardListNum]);
    UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentCardListNum);
}

void UShopCardWSubsystem::SetPreviousCard()
{
    if(0< CurrentCardListNum)
    {
        CurrentCardListNum--;
        for(int i =0; i < PlayerCardList.Num();i++)
        {
            if(0<CurrentCardListNum-1)
            {
                if(PlayerCardList[i].CardID == CardList[CurrentCardListNum].CardID)
                {
                    CurrentCardListNum--;
                }
            }
            
        }
    }
    
    OnCardChanged.Broadcast(CardList[CurrentCardListNum]);
    UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentCardListNum);
}

void UShopCardWSubsystem::SelectCard()
{
    
    for(int i =0; i< PlayerCardList.Num(); i++)
    {
        if(PlayerCardList[i].CardID ==-1)
        {
            PlayerCardList[i] = CardList[CurrentCardListNum];
            UE_LOG(LogTemp, Warning, TEXT("카드 추가 %d"), i);
            OnCardSelected.Broadcast(i, CardList[CurrentCardListNum]);
            return;
        }
    }
    
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