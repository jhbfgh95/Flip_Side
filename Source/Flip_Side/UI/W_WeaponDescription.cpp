// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_WeaponDescription.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

void UW_WeaponDescription::NativeConstruct()
{
    Super::NativeConstruct();

    SetExplainTextEmpty();
}

void UW_WeaponDescription::SetExplainText(const FString & WeaponName, const FString & Description, int32 DefaultBP, int32 DefaultAP)
{
    FFormatNamedArguments Args;

    if(DefaultBP != -1)
    {
        FString BPString = FString::Printf(TEXT("<White>%d</>"), DefaultBP);
        Args.Add(TEXT("BP"), FText::FromString(BPString));
    }
    if(DefaultAP != -1)
    {
        FString APString = FString::Printf(TEXT("<White>%d</>"), DefaultAP);
        Args.Add(TEXT("AP"), FText::FromString(APString));
    }
    

    FText DescriptionText = FText::FromString(Description);

    if (WeaponText)
    {
        WeaponText->SetText(FText::Format(DescriptionText, Args));
    }

    if (WeaponNameText)
    {
        WeaponNameText->SetText(FText::FromString(WeaponName));
    }

    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UW_WeaponDescription::SetExplainTextEmpty()
{
    if (WeaponText)
    {
        WeaponText->SetText(FText::GetEmpty());
    }

    if (WeaponNameText)
    {
        WeaponNameText->SetText(FText::GetEmpty());
    }

    SetVisibility(ESlateVisibility::Collapsed);
}
	
void UW_WeaponDescription::SetPanelStringText(const FString& Title, const FString& Descrip)
{
    if (WeaponNameText)
    {
        WeaponNameText->SetText(FText::FromString(Title));
    }

    if (WeaponText)
    {
        WeaponText->SetText(FText::FromString(Descrip));
    }

    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
