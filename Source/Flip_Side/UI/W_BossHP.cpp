// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BossHP.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UW_BossHP::NativeConstruct()
{
    Super::NativeConstruct();

    if(ClearImage) 
    {
        ClearImage->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UW_BossHP::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (IsHpAnimating)
    {
        AnimTime += InDeltaTime;

        float Alpha = AnimTime / Duration;
        Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

        float AnimPercent = FMath::Lerp(StartPercent, TargetPercent, Alpha);

        SetBossHpPercentage(AnimPercent);

        // 종료 처리
        if (Alpha >= 1.0f)
        {
            IsHpAnimating = false;
        }
    }
    
    if(IsShieldAnimating)
    {
        AnimTime += InDeltaTime;

        float Alpha = AnimTime / Duration;
        Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

        float AnimPercent = FMath::Lerp(StartPercent, TargetPercent, Alpha);

        SetBossShieldPercentage(AnimPercent);

        // 종료 처리
        if (Alpha >= 1.0f)
        {
            IsShieldAnimating = false;
        }
    }
}

void UW_BossHP::InitBossHp(int32 SetMaxHp)
{
    MaxHp = SetMaxHp;
    CurrentHp = SetMaxHp;
}

void UW_BossHP::InitBossShield(int32 SetMaxShield)
{
    MaxShield = SetMaxShield;
    CurrentShield = SetMaxShield;
    if(0<MaxShield)
        ShieldProgressBar->SetPercent(CurrentShield/MaxShield);

}

void UW_BossHP::ChangeMaxHp(int32 AddMaxHp)
{
    
    if(MaxHp + AddMaxHp < CurrentHp)
    {
        ChangeCurrentHp(AddMaxHp);
        return;
    }

    StartPercent = CurrentHp/MaxHp;

    if(MaxHp + AddMaxHp <=0)
    {
        TargetPercent = 0;
    }
    else
    {
        MaxHp += AddMaxHp;
    }

    TargetPercent = CurrentHp/MaxHp;

    StartHpAnimation();
}

void UW_BossHP::ChangeCurrentHp(int32 AddHpValue)
{
    StartPercent = (float)CurrentHp/MaxHp;

    if( MaxHp <= CurrentHp + AddHpValue )
    {
        CurrentHp = MaxHp;
    }
    else if(CurrentHp + AddHpValue<=0)
    {
        CurrentHp = 0;
        UE_LOG(LogTemp, Warning, TEXT("1111"));
    }
    else
    {
        CurrentHp += AddHpValue;
    }

    TargetPercent = (float)CurrentHp/MaxHp;

    StartHpAnimation();
}

	
void UW_BossHP::ChangeMaxShield(int32 AddMaxShield)
{
    if(MaxShield + AddMaxShield < CurrentShield)
    {
        ChangeCurrentShield(AddMaxShield);
        return;
    }

    StartPercent = CurrentShield/MaxShield;

    if(MaxShield + AddMaxShield <=0)
    {
        TargetPercent = 0;
    }
    else
    {
        MaxShield += AddMaxShield;
    }

    TargetPercent = (float)CurrentShield/MaxShield;

    StartShieldAnimation();
}

void UW_BossHP::ChangeCurrentShield(int32 AddShieldValue)
{
    StartPercent = (float)CurrentShield/MaxShield;

    if( MaxShield <= CurrentShield + AddShieldValue )
    {
        CurrentShield = MaxShield;
    }
    else if(CurrentShield + AddShieldValue<=0)
    {
        CurrentShield = 0;
    }
    else
    {
        CurrentShield += AddShieldValue;
    }

    TargetPercent = (float)CurrentShield/MaxShield;

    StartShieldAnimation();
}
	
void UW_BossHP::SetBossHpPercentage(float Perecetage)
{
    UE_LOG(LogTemp, Warning, TEXT("%f"), Perecetage);
    HpProgressBar->SetPercent(Perecetage);
}
	

void UW_BossHP::SetBossShieldPercentage(float Perecetage)
{
    
    ShieldProgressBar->SetPercent(Perecetage);
        
}


void UW_BossHP::StartHpAnimation()
{
    
	AnimTime = 0;
    IsHpAnimating = true;
}
void UW_BossHP::StartShieldAnimation()
{

	AnimTime = 0;
    IsShieldAnimating = true;
}

void UW_BossHP::ShowClearImage()
{
    if (ClearImage)
    {
        // 이미지를 화면에 보여줍니다 (클릭 이벤트를 무시하려면 HitTestInvisible 추천)
        ClearImage->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}