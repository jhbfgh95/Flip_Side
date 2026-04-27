// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BossHP.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

void UW_BossHP::NativeConstruct()
{
    Super::NativeConstruct();

    if(ClearImage) 
    {
        ClearImage->SetVisibility(ESlateVisibility::Hidden);
    }

    if(PatternHoverButton)
    {
        PatternHoverButton->OnHovered.AddDynamic(this, &UW_BossHP::ShowPatternPopup);
        PatternHoverButton->OnUnhovered.AddDynamic(this, &UW_BossHP::HidePatternPopup);
    }

    HidePatternPopup();
    RefreshHpBar();
    RefreshShieldBar();
    SnapHpBarToTarget();
    SnapShieldBarToTarget();
}

void UW_BossHP::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateProgressBars(InDeltaTime);
}

void UW_BossHP::InitBossHp(int32 SetMaxHp)
{
    MaxHp = FMath::Max(0, SetMaxHp);
    CurrentHp = MaxHp;

    RefreshHpBar();
    SnapHpBarToTarget();
}

void UW_BossHP::InitBossShield(int32 SetMaxShield)
{
    MaxShield = FMath::Max(0, SetMaxShield);
    CurrentShield = MaxShield;

    RefreshShieldBar();
    SnapShieldBarToTarget();
}

void UW_BossHP::SetBossName(const FString& SetBossName)
{
    if(BossNameText)
    {
        BossNameText->SetText(FText::FromString(SetBossName));
    }
}

void UW_BossHP::SetPatternInfo(int32 PatternDisplayIndex, const FString& PatternName, const FText& PatternDescription, int32 FinalDamage, UTexture2D* PatternIcon)
{
    if(PatternIndexText)
    {
        PatternIndexText->SetText(FText::AsNumber(PatternDisplayIndex));
    }

    if(PatternNameText)
    {
        PatternNameText->SetText(FText::FromString(PatternName));
    }

    if(PatternDescriptionText)
    {
        PatternDescriptionText->SetText(PatternDescription);
    }

    if(PatternDamageText)
    {
        PatternDamageText->SetText(FText::AsNumber(FinalDamage));
    }

    if(PatternIconImage)
    {
        if(PatternIcon)
        {
            PatternIconImage->SetBrushFromTexture(PatternIcon);
            PatternIconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        else
        {
            PatternIconImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UW_BossHP::ChangeMaxHp(int32 AddMaxHp)
{
    MaxHp = FMath::Max(0, MaxHp + AddMaxHp);
    CurrentHp = FMath::Clamp(CurrentHp, 0, MaxHp);

    RefreshHpBar();
}

void UW_BossHP::ChangeCurrentHp(int32 AddHpValue)
{
    CurrentHp = FMath::Clamp(CurrentHp + AddHpValue, 0, MaxHp);

    RefreshHpBar();
}

	
void UW_BossHP::ChangeMaxShield(int32 AddMaxShield)
{
    MaxShield = FMath::Max(0, MaxShield + AddMaxShield);
    CurrentShield = FMath::Clamp(CurrentShield, 0, MaxShield);

    RefreshShieldBar();
}

void UW_BossHP::ChangeCurrentShield(int32 AddShieldValue)
{
    CurrentShield = FMath::Clamp(CurrentShield + AddShieldValue, 0, MaxShield);

    RefreshShieldBar();
}


void UW_BossHP::ShowClearImage()
{
    if (ClearImage)
    {
        // 이미지를 화면에 보여줍니다 (클릭 이벤트를 무시하려면 HitTestInvisible 추천)
        ClearImage->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UW_BossHP::ShowPatternPopup()
{
    if(PatternPopupPanel)
    {
        PatternPopupPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UW_BossHP::HidePatternPopup()
{
    if(PatternPopupPanel)
    {
        PatternPopupPanel->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UW_BossHP::RefreshHpBar()
{
    if(HpText)
    {
        HpText->SetText(FText::AsNumber(CurrentHp));
    }

    if(HpTotalText)
    {
        HpTotalText->SetText(FText::Format(
            FText::FromString(TEXT("/ {0}")),
            FText::AsNumber(MaxHp)
        ));
    }

    if(HPProgressBar)
    {
        TargetHpPercent = GetHpPercent();
        if(!bIsHpBarInitialized)
        {
            SnapHpBarToTarget();
        }
    }
}

void UW_BossHP::RefreshShieldBar()
{
    if(ShieldText)
    {
        ShieldText->SetText(FText::AsNumber(CurrentShield));
    }

    if(ShieldTotalText)
    {
        ShieldTotalText->SetText(FText::Format(
            FText::FromString(TEXT("/ {0}")),
            FText::AsNumber(MaxShield)
        ));
        ShieldTotalText->SetVisibility(CurrentShield > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
    }

    if(ShieldProgressBar)
    {
        TargetShieldPercent = GetShieldPercent();
        if(!bIsShieldBarInitialized)
        {
            SnapShieldBarToTarget();
        }
    }

}

void UW_BossHP::UpdateProgressBars(float InDeltaTime)
{
    if(HPProgressBar)
    {
        DisplayHpPercent = FMath::FInterpTo(DisplayHpPercent, TargetHpPercent, InDeltaTime, ProgressBarInterpSpeed);

        if(FMath::IsNearlyEqual(DisplayHpPercent, TargetHpPercent, 0.001f))
        {
            DisplayHpPercent = TargetHpPercent;
        }

        HPProgressBar->SetPercent(DisplayHpPercent);
    }

    if(ShieldProgressBar)
    {
        DisplayShieldPercent = FMath::FInterpTo(DisplayShieldPercent, TargetShieldPercent, InDeltaTime, ProgressBarInterpSpeed);

        if(FMath::IsNearlyEqual(DisplayShieldPercent, TargetShieldPercent, 0.001f))
        {
            DisplayShieldPercent = TargetShieldPercent;
        }

        ShieldProgressBar->SetPercent(DisplayShieldPercent);
    }

}

void UW_BossHP::SnapHpBarToTarget()
{
    TargetHpPercent = GetHpPercent();
    DisplayHpPercent = TargetHpPercent;
    bIsHpBarInitialized = true;

    if(HPProgressBar)
    {
        HPProgressBar->SetPercent(DisplayHpPercent);
    }
}

void UW_BossHP::SnapShieldBarToTarget()
{
    TargetShieldPercent = GetShieldPercent();
    DisplayShieldPercent = TargetShieldPercent;
    bIsShieldBarInitialized = true;

    if(ShieldProgressBar)
    {
        ShieldProgressBar->SetPercent(DisplayShieldPercent);
    }

}

float UW_BossHP::GetHpPercent() const
{
    if(MaxHp <= 0)
    {
        return 0.f;
    }

    return static_cast<float>(CurrentHp) / static_cast<float>(MaxHp);
}

float UW_BossHP::GetShieldPercent() const
{
    if(MaxShield <= 0)
    {
        return 0.f;
    }

    return static_cast<float>(CurrentShield) / static_cast<float>(MaxShield);
}
