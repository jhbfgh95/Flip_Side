// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BossHpElement.h"

#include "Components/Image.h"
#include "Components/Widget.h"
#include "TimerManager.h"

void UW_BossHpElement::NativeConstruct()
{
    Super::NativeConstruct();

    HpMID = ElementImage->GetDynamicMaterial();
    ShieldMID = ShieldElementImage->GetDynamicMaterial();
    HpDamageMID = HpDamageImage ? HpDamageImage->GetDynamicMaterial() : nullptr;
    ShieldDamageMID = ShieldDamageImage ? ShieldDamageImage->GetDynamicMaterial() : nullptr;

    HideImage(HpDamageImage);
    HideImage(ShieldDamageImage);
    ShowImage(ElementImage);
    ShowImage(ShieldElementImage);
    ShowWidget(HPBorder);
    RefreshShieldVisibility();
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

void UW_BossHpElement::SetHpDamageMaterial(int32 HpLevel)
{
    if(HpLevelColor.Num()<=0)
        return;
    
    if(HpDamageMID)
    {
        if(HpLevelColor.Num()<=HpLevel)
            HpDamageMID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[HpLevelColor.Num()-1]);
        else if(HpLevel<=-1)
            HpDamageMID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[0]);
        else
            HpDamageMID->SetVectorParameterValue(TEXT("HpColor"), HpLevelColor[HpLevel]);
    }
}

void UW_BossHpElement::SetShieldDamageMaterial(int32 ShieldLevel)
{
    if(ShieldLevelColor.Num()<=0)
        return;
    
    if(ShieldDamageMID)
    {
        if(ShieldLevelColor.Num()<=ShieldLevel)
            ShieldDamageMID->SetVectorParameterValue(TEXT("ShieldColor"), ShieldLevelColor[ShieldLevelColor.Num()-1]);
        else if(ShieldLevel<=-1)
            ShieldDamageMID->SetVectorParameterValue(TEXT("ShieldColor"), ShieldLevelColor[0]);
        else
            ShieldDamageMID->SetVectorParameterValue(TEXT("ShieldColor"), ShieldLevelColor[ShieldLevel]);
    }
}

void UW_BossHpElement::InitElementHp(int32 HpValue)
{
    ApplyPendingHpDecrease();
    ElementHp = HpValue;
    SetHpMaterial(ElementHp);
}

void UW_BossHpElement::InitElementShield(int32 ShieldValue)
{
    ApplyPendingShieldDecrease();
    ElementShield = ShieldValue;
    SetShieldMaterial(ElementShield);
    RefreshShieldVisibility();
}

void UW_BossHpElement::DecreaseHp()
{
    PendingHpDecreaseCount++;
    SetHpDamageMaterial(ElementHp);
    HideImage(ElementImage);
    ShowImage(HpDamageImage);

    if(DamageRemainTime <= 0.f || !GetWorld())
    {
        ApplyPendingHpDecrease();
        return;
    }

    GetWorld()->GetTimerManager().SetTimer(
        HpDamageTimerHandle,
        this,
        &UW_BossHpElement::ApplyPendingHpDecrease,
        DamageRemainTime,
        false
    );
}
	
void UW_BossHpElement::IncreaseHp()
{
    ApplyPendingHpDecrease();
    ElementHp++;
    SetHpMaterial(ElementHp);
}	

void UW_BossHpElement::DecreaseShield()
{
    if(ElementShield <= 0)
    {
        RefreshShieldVisibility();
        return;
    }

    PendingShieldDecreaseCount++;
    SetShieldDamageMaterial(ElementShield);
    HideImage(ShieldElementImage);
    HideWidget(ShieldBorder);
    ShowImage(ShieldDamageImage);

    if(DamageRemainTime <= 0.f || !GetWorld())
    {
        ApplyPendingShieldDecrease();
        return;
    }

    GetWorld()->GetTimerManager().SetTimer(
        ShieldDamageTimerHandle,
        this,
        &UW_BossHpElement::ApplyPendingShieldDecrease,
        DamageRemainTime,
        false
    );
}

void UW_BossHpElement::IncreaseShield()
{
    ApplyPendingShieldDecrease();
    ElementShield++;
    SetShieldMaterial(ElementShield);
    RefreshShieldVisibility();
}

void UW_BossHpElement::ShowImage(UImage* Image)
{
    if(Image)
    {
        Image->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UW_BossHpElement::HideImage(UImage* Image)
{
    if(Image)
    {
        Image->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UW_BossHpElement::ShowWidget(UWidget* Widget)
{
    if(Widget)
    {
        Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UW_BossHpElement::HideWidget(UWidget* Widget)
{
    if(Widget)
    {
        Widget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UW_BossHpElement::RefreshShieldVisibility()
{
    if(ElementShield <= 0)
    {
        HideImage(ShieldElementImage);
        HideImage(ShieldDamageImage);
        HideWidget(ShieldBorder);
        return;
    }

    ShowImage(ShieldElementImage);
    HideImage(ShieldDamageImage);
    ShowWidget(ShieldBorder);
}

void UW_BossHpElement::ApplyPendingHpDecrease()
{
    if(PendingHpDecreaseCount <= 0)
    {
        return;
    }

    if(GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HpDamageTimerHandle);
    }

    ElementHp = FMath::Max(0, ElementHp - PendingHpDecreaseCount);
    PendingHpDecreaseCount = 0;

    SetHpMaterial(ElementHp);
    HideImage(HpDamageImage);
    ShowImage(ElementImage);
}

void UW_BossHpElement::ApplyPendingShieldDecrease()
{
    if(PendingShieldDecreaseCount <= 0)
    {
        return;
    }

    if(GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ShieldDamageTimerHandle);
    }

    ElementShield = FMath::Max(0, ElementShield - PendingShieldDecreaseCount);
    PendingShieldDecreaseCount = 0;

    SetShieldMaterial(ElementShield);
    RefreshShieldVisibility();
}
