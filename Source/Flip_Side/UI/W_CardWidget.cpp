// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CardWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
    const FName WCardIconParameterName(TEXT("Weapon_Icon"));
    const FName WCardColorParameterName(TEXT("Weapon_Color"));
    const FLinearColor WCardIconColor(1.0f, 0.823529f, 1.0f, 1.0f); // FFD2FFFF
}


void UW_CardWidget::NativeConstruct()
{
    Super::NativeConstruct();


}

void UW_CardWidget::InitCard(FCardData CardData)
{
    if (IsValid(CardIconImage) && IsValid(CardData.Icon))
    {
        // CardIconImage의 BP Brush에 지정한 공용 UI 머티리얼을 유지하고 파라미터만 갱신합니다.
        CardIconMaterialInstance = CardIconImage->GetDynamicMaterial();
        if (IsValid(CardIconMaterialInstance))
        {
            CardIconMaterialInstance->SetTextureParameterValue(WCardIconParameterName, CardData.Icon);
            CardIconMaterialInstance->SetVectorParameterValue(WCardColorParameterName, WCardIconColor);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[CardInfo] GetDynamicMaterial failed. CardIconImage Brush에 공용 UI 머티리얼을 지정하세요."));
        }
    }

    if (IsValid(CardTitle))
    {
        CardTitle->SetText(FText::FromString(CardData.CardName));
    }
    
    if (IsValid(CardDescription))
    {
        CardDescription->SetText(FText::FromString(CardData.Card_Description));
    }
}
