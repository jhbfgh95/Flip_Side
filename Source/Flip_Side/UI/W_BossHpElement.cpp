// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BossHpElement.h"

#include "Components/Image.h"

void UW_BossHpElement::NativeConstruct()
{
    Super::NativeConstruct();

    MID = ElementImage->GetDynamicMaterial();
}

void UW_BossHpElement::SetHpMaterial(int32 HpLevel)
{
    if(HpLevelColor.Num()<=0)
        return;
    
    if(MID)
    {
        if(HpLevelColor.Num()<=HpLevel)
            MID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[HpLevelColor.Num()-1]);
        else if(HpLevel<=-1)
            MID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[0]);
        else
            MID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[HpLevel]);
    }
}

void UW_BossHpElement::InitElementHp(int32 HpValue)
{
    ElementHp = HpValue;
    SetHpMaterial(ElementHp);
}

void UW_BossHpElement::DecreaseHp()
{
    ElementHp--;
    SetHpMaterial(ElementHp+1);
}
	
void UW_BossHpElement::IncreaseHp()
{
    ElementHp++;
    SetHpMaterial(ElementHp+1);
}