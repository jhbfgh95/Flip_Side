// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"

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
    
    
    FCardData DefaultCard;
    
    for(int i =0; i<6; i++)
    {
        DefaultCard.CardID = i;
        DefaultCard.CardName = FString::FromInt(i);
        DefaultCard.Card_Description= FString::FromInt(i);
        UnlockCardList.Add(DefaultCard);
    }

    for(int i =0; i<3; i++)
    {
        DefaultCard.CardID = -1;
        PlayerCardList.Add(DefaultCard);
    }

    if(0<UnlockCardList.Num())
    {
        OnCardChanged.Broadcast(UnlockCardList[CurrentCardListNum]);
    }
    
}

void UShopCardWSubsystem::OnWorldBeginPlay(UWorld& World)
{
    Super::OnWorldBeginPlay(World);
    /*
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        DM = GI->GetSubsystem<UDataManagerSubsystem>();
    }
    if(DM)
    {
        //TryGetCard나오면 받아오도록 추후 추가
        FCardData CardData;
        for(int i = 0; i<6 ; i++)
        {
            if(DM->TryGetCard(0,CardData))
            {
                UnlockCardList.Add(CardData);
            }
        }
        
    }
    */
}

void UShopCardWSubsystem::SetNextCard()
{
    //다음 버튼을 눌렀을 때 끝인지 검사
    if(UnlockCardList.Num() <= CurrentCardListNum+1 )
    {
        UE_LOG(LogTemp, Warning, TEXT("끝에 도달함"));
        return;
    }
    CurrentCardListNum++;
    OnCardChanged.Broadcast(UnlockCardList[CurrentCardListNum]);
}

void UShopCardWSubsystem::SetPreviousCard()
{
    if(CurrentCardListNum-1 < 0 )
    {
        UE_LOG(LogTemp, Warning, TEXT("끝에 도달함"));
        return;
    }

    CurrentCardListNum--;
    OnCardChanged.Broadcast(UnlockCardList[CurrentCardListNum]);
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
            
            PlayerCardList[i] = UnlockCardList[CurrentCardListNum];
            OnCardSelected.Broadcast(i, PlayerCardList[i]);
            return;
        }
    }
    
}

bool UShopCardWSubsystem::CanSelectCard()
{
    for(int i =0; i<PlayerCardList.Num(); i++)
    {
        if(PlayerCardList[i].CardID == UnlockCardList[CurrentCardListNum].CardID)
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
    if(0<CurrentCardListNum)
        return UnlockCardList[CurrentCardListNum];
    else
    {
        FCardData DefaultCard;
        DefaultCard.CardID =-1;
        return DefaultCard;
    }
          
}


void UShopCardWSubsystem::RemoveHandCard(int32 HandIndex)
{
    if(HandIndex<PlayerCardList.Num())
    {
        PlayerCardList[HandIndex].CardID = -1;
        UE_LOG(LogTemp, Warning, TEXT("삭제한 번호 아이디%d"),PlayerCardList[HandIndex].CardID);
    }
}


int32 UShopCardWSubsystem::GetPlayerCardID(int32 index)
{
    if(index<PlayerCardList.Num())
        return PlayerCardList[index].CardID;
    else 
        return -1;
}