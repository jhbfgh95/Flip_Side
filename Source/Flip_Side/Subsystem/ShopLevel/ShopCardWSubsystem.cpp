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
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        DM = GI->GetSubsystem<UDataManagerSubsystem>();
    }

    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    SetUnlockCard();

    UnlockSubsystem->OnUnlockCard.AddDynamic(this,&UShopCardWSubsystem::AddCardListToUnlockCard);

    DefaultCard.CardID =-1;
    
    for(int i =0; i<3; i++)
        PlayerCardList.Add(DefaultCard);
}

void UShopCardWSubsystem::Deinitialize()
{
    UnlockSubsystem->OnUnlockCard.RemoveAll(this);
    Super::Deinitialize();
}

void UShopCardWSubsystem::SetNextCard()
{
    if(UnlockCardList.Num()<=0)
        return;

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
    if(UnlockCardList.Num()<=0)
        return;
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
    
    if(0<=CurrentCardListNum)
    {
        if(CurrentCardListNum<UnlockCardList.Num())
            return UnlockCardList[CurrentCardListNum];
        else
            return DefaultCard;

    }
    else
    {
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