// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ExplainWidget.h"
#include "Components/TextBlock.h"


void UW_ExplainWidget::SetExplainText(FString InputText)
{
    ExplainText->SetText(FText::FromString(InputText));
}
