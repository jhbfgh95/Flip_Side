// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CoinHPWidget.h"
#include "Components/Image.h"


void UW_CoinHPWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (IsValid(HpImage))
    {
        MID = HpImage->GetDynamicMaterial();
        SetHpPrgressBar(MaxHp > 0 ? static_cast<float>(CurrentHp) / MaxHp : 0.0f);
    }

}
void UW_CoinHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!IsHpAnimating) return;

    AnimTime += InDeltaTime;

    float Alpha = AnimTime / Duration;
    Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

    float AnimHpPercent = FMath::Lerp(StartHpPercent, TargetHpPercent, Alpha);

    SetHpPrgressBar(AnimHpPercent);

    // 종료 처리
    if (Alpha >= 1.0f)
    {
        IsHpAnimating = false;
    }
}

void UW_CoinHPWidget::InitHpWidget(int32 MaxHpValue, int32 CurrentHpValue)
{
    SetMaxHp(FMath::Max(1, MaxHpValue));
    SetCurrentHp(FMath::Clamp(CurrentHpValue, 0, MaxHp));
    IsHpAnimating = false;
    SetHpPrgressBar(static_cast<float>(CurrentHp) / MaxHp);
}
	
void UW_CoinHPWidget::SetMaxHp(int32 MaxHpValue)
{
    MaxHp = MaxHpValue;
}

void UW_CoinHPWidget::SetCurrentHp(int32 Hpvalue)
{
    CurrentHp = Hpvalue;
}

void UW_CoinHPWidget::ChangeMaxHp(int32 Hpvalue)
{
    StartHpPercent = MaxHp > 0 ? static_cast<float>(CurrentHp) / MaxHp : 0.0f;
    MaxHp = FMath::Max(1, MaxHp + Hpvalue);
    // 현재 HP 변화는 뒤이어 전달되는 OnHpChanged에서 한 번만 반영합니다.
    TargetHpPercent = FMath::Clamp(static_cast<float>(CurrentHp) / MaxHp, 0.0f, 1.0f);
    StartHpAnimation();
}

void UW_CoinHPWidget::ChangeCurrentHp(int32 HPModifier)
{
    if (MaxHp <= 0) 
    {
        return;
    } 
    StartHpPercent = static_cast<float>(CurrentHp)/MaxHp;

    CurrentHp = FMath::Clamp(CurrentHp + HPModifier, 0, MaxHp);

    TargetHpPercent =  static_cast<float>(CurrentHp)/MaxHp;

    StartHpAnimation();
}
	
void UW_CoinHPWidget::SetHpPrgressBar(float Percentage)
{
    if(IsValid(MID))
    {
        MID->SetScalarParameterValue(TEXT("HpPercent"), Percentage);
    }
}

void UW_CoinHPWidget::StartHpAnimation()
{
	AnimTime = 0;
    IsHpAnimating = true;
}
