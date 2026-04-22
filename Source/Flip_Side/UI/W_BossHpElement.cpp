// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BossHpElement.h"

#include "Components/Image.h"

void UW_BossHpElement::NativeConstruct()
{
    Super::NativeConstruct();

    HpMID = ElementImage->GetDynamicMaterial();
    ShieldMID = ShieldElementImage->GetDynamicMaterial();
}

void UW_BossHpElement::SetHpMaterial(int32 HpLevel)
{
    if(HpLevelColor.Num()<=0)
        return;
    
    if(HpMID)
    {
        if(HpLevelColor.Num()<=HpLevel)
            HpMID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[HpLevelColor.Num()-1]);
        else if(HpLevel<=-1)
            HpMID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[0]);
        else
            HpMID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[HpLevel]);
    }
}

void UW_BossHpElement::SetShieldMaterial(int32 ShieldLevel)
{
    if(ShieldLevelColor.Num()<=0)
        return;
    
    UE_LOG(LogTemp, Warning, TEXT("개수 %d / 보호막 값 %d"), ShieldLevelColor.Num(),ShieldLevel);
    if(ShieldMID)
    {
        if(ShieldLevelColor.Num()<=ShieldLevel)
            ShieldMID->SetVectorParameterValue(TEXT("ShieldColor"), ShieldLevelColor[ShieldLevelColor.Num()-1]);
        else if(ShieldLevel<=-1)
            ShieldMID->SetVectorParameterValue(TEXT("ShieldColor"), ShieldLevelColor[0]);
        else
            ShieldMID->SetVectorParameterValue(TEXT("ShieldColor"), ShieldLevelColor[ShieldLevel]);
    }
}

void UW_BossHpElement::InitElementHp(int32 HpValue)
{
    ElementHp = HpValue;
    SetHpMaterial(ElementHp);
}

void UW_BossHpElement::InitElementShield(int32 ShieldValue)
{
    
    UE_LOG(LogTemp, Warning, TEXT("보호막 설정 시작"));
    ElementShield = ShieldValue;
    SetShieldMaterial(ElementHp);
}

void UW_BossHpElement::DecreaseHp()
{
    ElementHp--;
    SetHpMaterial(ElementHp);
}
	
void UW_BossHpElement::IncreaseHp()
{
    ElementHp++;
    SetHpMaterial(ElementHp);
}	

void UW_BossHpElement::DecreaseShield()
{
    ElementShield--;
    SetShieldMaterial(ElementShield);
}

void UW_BossHpElement::IncreaseShield()
{
    ElementShield++;
    SetShieldMaterial(ElementShield);
}