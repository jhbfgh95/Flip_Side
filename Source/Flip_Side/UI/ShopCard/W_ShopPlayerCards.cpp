// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerCards.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "UI/ShopCard/W_ShopPlayerCard.h"
#include "Components/CanvasPanel.h"
void UW_ShopPlayerCards::NativeConstruct()
{
    Super::NativeConstruct();/*
    ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();

    ShopCardSubsystem->OnUnSelectPlayerCard.AddDynamic(this, &UW_ShopPlayerCards::SetUnSelectPlayerCard);
    //ShopCardSubsystem->OnSelectPlayerCard.AddDynamic(this, &UW_ShopPlayerCards::SetSelectPlayerCard);

    int32 ChildCount = MainCanvas->GetChildrenCount();

    DefaultCardData.CardID = -1;
    for (int32 i = 0; i < ChildCount; ++i)
    {
        UW_ShopPlayerCard* PlayerCardData = Cast<UW_ShopPlayerCard>(MainCanvas->GetChildAt(i));
        if(PlayerCardData != nullptr)
            PlayerCardWidget.Add(PlayerCardData);

        PlayerCardData->InitPlayerCard(DefaultCardData);
    }
*/

}
void UW_ShopPlayerCards::NativeDestruct()
{
    //ShopCardSubsystem->OnChangePlayerCard.RemoveAll(this);
    Super::NativeDestruct();
}
	
void UW_ShopPlayerCards::SetUnSelectPlayerCard(FCardData UnSelectCardData, int32 UnSelectCardIndex)
{
    /*
    if(UnSelectCardIndex == -1)
        return;

    if(UnSelectCardIndex < PlayerCardWidget.Num())
    {
        PlayerCardWidget[UnSelectCardIndex]->InitPlayerCard(DefaultCardData);
    }*/
        
}
	
void UW_ShopPlayerCards::SetSelectPlayerCard(FCardData CardData)
{
    //int Index= ShopCardSubsystem->GetPlayerCardIndexByID(CardData.CardID);
    //if(Index != -1 && PlayerCardWidget.IsValidIndex(Index))
        //PlayerCardWidget[Index]->InitPlayerCard(CardData);
}