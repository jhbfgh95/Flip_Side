// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_UnlockCardWidget.h"

#include "Subsystem/ShopCardWSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Components/Image.h"

void UW_UnlockCardWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UnlockSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUnlockGISubsystem>();

    
}

void UW_UnlockCardWidget::InitUnlockCard(FCardData CardData)
{
    if(UnlockSubsystem->IsCardUnlockByID(CardData.CardID))
    {
        LockImage->SetVisibility(ESlateVisibility::Hidden);
    }
    InitCard(CardData);
}