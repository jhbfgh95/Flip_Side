// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemGridContainer.h"


#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Components/Button.h"
void UW_ShopItemGridContainer::NativeConstruct()
{
    Super::NativeConstruct();
    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();

    ShopButton->OnClicked.AddDynamic(this ,&UW_ShopItemGridContainer::SetShopGrid);
    ItemInvenButton->OnClicked.AddDynamic(this ,&UW_ShopItemGridContainer::SetInvenGrid);

}

	
void UW_ShopItemGridContainer::SetShopGrid()
{
    ItemInvenGrid->SetVisibility(ESlateVisibility::Hidden);
    ShopGrid->SetVisibility(ESlateVisibility::Visible);
}
    
void UW_ShopItemGridContainer::SetInvenGrid()
{
    ShopGrid->SetVisibility(ESlateVisibility::Hidden);
    ItemInvenGrid->SetVisibility(ESlateVisibility::Visible);
}