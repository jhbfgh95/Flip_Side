// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_WeaponDescription.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
void UW_WeaponDescription::SetExplainText(const FString & WeaponName, const FString & Description, int32 DefaultBP, int32 DefaultAP)
{
    FFormatNamedArguments Args;

    //언리얼 기본 포맷 
    //사용 : {BP} 만큼 데미지를 줍니다
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

    WeaponText->SetText(FText::Format(DescriptionText, Args));
    WeaponNameText->SetText(FText::FromString(WeaponName));
}

void UW_WeaponDescription::SetExplainTextEmpty()
{
    
    WeaponText->SetText(FText::GetEmpty());
    WeaponNameText->SetText(FText::GetEmpty());
}