// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopLevel/ShopUnlockCardWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
bool UShopUnlockCardWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UShopUnlockCardWSubsystem::OnWorldBeginPlay(UWorld& World)
{
    Super::OnWorldBeginPlay(World);

    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();
    DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    DefaultCardData.CardID = -1;
    CurrentSelectCardID =-1;
    DataManager->TryGetAllCards(CardList);
}

void UShopUnlockCardWSubsystem::InitUnlockCard()
{
    //1. 카드 데이터를 받아서 배열에 저장함
    //2. UnlockGI, 접근해서 값이 있는지 받아옴
    //3. 해금된 값을 계속 탐색?
}


FCardData UShopUnlockCardWSubsystem::GetCurrnetPageCardData(int32 Index)
{
    if(0<=CurrentPage+Index &&CurrentPage+Index<CardList.Num())
    {
        return CardList[CurrentPage+Index];
    }
    return DefaultCardData;
}

FCardData UShopUnlockCardWSubsystem::GetCardDataByIndex(int32 Index)
{
    if(0<=Index &&Index<CardList.Num())
    {
        return CardList[Index];
    }
    return DefaultCardData;
}


bool UShopUnlockCardWSubsystem::UnlockCard()
{
    /*
    if(돈이 있는지?)
    {
        UnlockSubsystem->Add(ID);
        return true;
    }
    else
    {
        return false;
    }
    */
   UnlockSubsystem->UnlockCard(CurrentSelectCardID);
   OnUnlockSelectCard.Broadcast(CurrentSelectCardID);
   CurrentSelectCardID = -1;
   return true;
}
void UShopUnlockCardWSubsystem::ChangeCardsLeft()
{
    if(0<=CurrentPage-4)
    {
        CurrentPage-=4;
        CurrentSelectCardID = -1;
        OnChangelockCardsLeft.Broadcast();
    }

}
void UShopUnlockCardWSubsystem::ChangeCardsRight()
{
    if(CurrentPage+4 <CardList.Num())
    {
        CurrentPage+=4;
        CurrentSelectCardID = -1;
        OnChangelockCardsRight.Broadcast();
    }
}

void UShopUnlockCardWSubsystem::SelectCard(int32 SelectCardID)
{
    if(CurrentSelectCardID != SelectCardID)
    {
        OnUnSelectUnlockCard.Broadcast(CurrentSelectCardID);
        CurrentSelectCardID = SelectCardID;
        OnSelectUnlockCard.Broadcast(SelectCardID);
    }
    
}

void UShopUnlockCardWSubsystem::UnSelectCard()
{
    if(CurrentSelectCardID != -1)
    {
        OnUnSelectUnlockCard.Broadcast(CurrentSelectCardID);
        CurrentSelectCardID = -1;
    }
}