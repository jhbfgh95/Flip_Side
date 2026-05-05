// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_WarningWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UW_WarningWidget::ActiveWarning(FString Text)
{
    WarningImage->SetVisibility(ESlateVisibility::Visible);
    WarningText->SetText(FText::FromString(Text));
    PlayAnimation(WarningTextAnim);
}