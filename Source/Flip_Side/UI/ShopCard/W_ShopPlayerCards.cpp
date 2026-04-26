// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopPlayerCards.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"
#include "UI/ShopCard/W_ShopCardWidget.h"
#include "Components/CanvasPanel.h"
void UW_ShopPlayerCards::NativeConstruct()
{
    Super::NativeConstruct();
    ShopCardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    ShopCardSubsystem->OnChangePlayerCard.AddDynamic(this, &UW_ShopPlayerCards::SetPlayerCard);

    int32 ChildCount = MainCanvas->GetChildrenCount();

    for (int32 i = 0; i < ChildCount; ++i)
    {
        UW_ShopCardWidget* PlayerCardData = Cast<UW_ShopCardWidget>(MainCanvas->GetChildAt(i));
        PlayerCardWidget.Add(PlayerCardData);
    }

}
void UW_ShopPlayerCards::NativeDestruct()
{
    ShopCardSubsystem->OnChangePlayerCard.RemoveAll(this);
    Super::NativeDestruct();
}
	
void UW_ShopPlayerCards::SetPlayerCard()
{
    TArray<FCardData> PlayerCardData = ShopCardSubsystem->GetPlayerCardList();
    
    for(int i =0; i < PlayerCardData.Num(); i++)
    {
        if(i<PlayerCardWidget.Num())
            PlayerCardWidget[i]->InitShopCard(PlayerCardData[i],true);
    }
}