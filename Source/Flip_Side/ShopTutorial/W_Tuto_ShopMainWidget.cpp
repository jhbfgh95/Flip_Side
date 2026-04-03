// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_ShopMainWidget.h"
#include "Player/GameMode_ShopTuto.h"
#include "Components/Button.h"
void UW_Tuto_ShopMainWidget::NativeConstruct()
{
    Super::NativeConstruct();

    TutoGameMode = Cast<AGameMode_ShopTuto>(GetWorld()->GetAuthGameMode());

    GameStartButton->OnClicked.RemoveAll(this);
    GameStartButton->OnClicked.AddDynamic(this,&UW_Tuto_ShopMainWidget::ClickBattleStartButton);

    
}

void UW_Tuto_ShopMainWidget::ClickBattleStartButton()
{
    if(TutoGameMode)
        TutoGameMode->ChangeBattleLevel();
}