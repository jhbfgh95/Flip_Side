// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CoinHPWidget.h"
#include "Components/Image.h"


void UW_CoinHPWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UMaterialInterface* Mat = Cast<UMaterialInterface>(HpImage->GetBrush().GetResourceObject());
    MID = UMaterialInstanceDynamic::Create(Mat, this);
    HpImage->SetBrushFromMaterial(MID);

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

void UW_CoinHPWidget::InitHpWidget(int32 MaxHpValue)
{
    SetMaxHp(MaxHpValue);
    SetCurrentHp(MaxHpValue);
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
    if(MaxHp + Hpvalue < CurrentHp)
    {
        ChangeCurrentHp(Hpvalue);
        return;
    }

    StartHpPercent =  static_cast<float>(CurrentHp)/MaxHp;

    if(MaxHp + Hpvalue <=0)
    {
        TargetHpPercent = 0;
    }
    else
    {
        MaxHp += Hpvalue;
    }

    TargetHpPercent =  static_cast<float>(CurrentHp)/MaxHp;

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
    if(MID)
    {
        MID->SetScalarParameterValue(TEXT("HpPercent"), Percentage);
    }
}

void UW_CoinHPWidget::StartHpAnimation()
{
	AnimTime = 0;
    IsHpAnimating = true;
}