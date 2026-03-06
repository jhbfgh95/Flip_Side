// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CoinManagePanelWidget.h"
#include "Components/TextBlock.h"



void UW_CoinManagePanelWidget::SetCountText(int32 Count)
{
    int32 TenNum = Count/10;
    int32 OneNum = Count%10;
    
    UE_LOG(LogTemp, Warning, TEXT("%d / %d"), TenNum, OneNum );
    TenText->SetText(FText::AsNumber(TenNum));
    OneText->SetText(FText::AsNumber(OneNum));
}
