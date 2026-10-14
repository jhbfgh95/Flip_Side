// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BossHP.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

void UW_BossHP::NativeConstruct()
{
    Super::NativeConstruct();

    // 둥근 창의 기본 브러시(문양 머티리얼)를 기억해 두고, 초상화와 문양을 오갈 때 씁니다.
    if (IsValid(BossEmblemImage))
    {
        EmblemMaterial = Cast<UMaterialInterface>(BossEmblemImage->GetBrush().GetResourceObject());
    }

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

void UW_BossHP::SetBossHUDData(const FBossHUDData& InData)
{
    SetBossName(InData.BossName);
    UpdateBossWindow(InData);

    MaxHp = FMath::Max(0, InData.MaxHP);
    CurrentHp = FMath::Clamp(InData.CurrentHP, 0, MaxHp);
    RefreshHpBar();

    if (InData.bUseGroggyAsShield)
    {
        const int32 NewMaxGroggy = FMath::Max(0, InData.MaxGroggy);
        if (!bDisplayingGroggyAsShield || MaxShield != NewMaxGroggy)
        {
            InitGroggyAsShield(NewMaxGroggy);
        }

        bDisplayingGroggyAsShield = true;
        UpdateGroggyAsShield(InData.CurrentGroggy);
    }
    else
    {
        bDisplayingGroggyAsShield = false;
        MaxShield = FMath::Max(0, InData.MaxShield);
        CurrentShield = FMath::Clamp(InData.CurrentShield, 0, MaxShield);
        RefreshShieldBar();
    }

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

void UW_BossHP::InitGroggyAsShield(int32 SetMaxGroggy)
{
    MaxShield = FMath::Max(0, SetMaxGroggy);
    CurrentShield = 0;

    if (ShieldText)
        ShieldText->SetText(FText::AsNumber(0));
    if (ShieldTotalText)
    {
        ShieldTotalText->SetText(FText::Format(FText::FromString(TEXT("/ {0}")), FText::AsNumber(MaxShield)));
        ShieldTotalText->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    if (ShieldProgressBar)
        ShieldProgressBar->SetFillColorAndOpacity(FLinearColor(1.f, 0.6f, 0.f, 1.f));

    // 그로기는 1씩 쌓이므로 눈금도 최대치에 맞춰 다시 그립니다.
    if (ShieldTickPanel && ShieldTickMax != MaxShield)
    {
        RebuildTicks(ShieldTickPanel, MaxShield);
        ShieldTickMax = MaxShield;
    }

    TargetShieldPercent = 0.f;
    SnapShieldBarToTarget();
}

void UW_BossHP::UpdateGroggyAsShield(int32 NewCurrentGroggy)
{
    CurrentShield = FMath::Clamp(NewCurrentGroggy, 0, MaxShield);

    if (ShieldText)
        ShieldText->SetText(FText::AsNumber(CurrentShield));

    if (ShieldProgressBar)
    {
        TargetShieldPercent = MaxShield > 0 ? static_cast<float>(CurrentShield) / static_cast<float>(MaxShield) : 0.f;
    }
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

    if (HpTickPanel && HpTickMax != MaxHp)
    {
        RebuildTicks(HpTickPanel, MaxHp);
        HpTickMax = MaxHp;
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

    if (ShieldTickPanel && ShieldTickMax != MaxShield)
    {
        RebuildTicks(ShieldTickPanel, MaxShield);
        ShieldTickMax = MaxShield;
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
        UpdateLabelPosition(HpLabelBox, HPProgressBar, DisplayHpPercent, HpLabelX, InDeltaTime);
    }

    if(ShieldProgressBar)
    {
        DisplayShieldPercent = FMath::FInterpTo(DisplayShieldPercent, TargetShieldPercent, InDeltaTime, ProgressBarInterpSpeed);

        if(FMath::IsNearlyEqual(DisplayShieldPercent, TargetShieldPercent, 0.001f))
        {
            DisplayShieldPercent = TargetShieldPercent;
        }

        ShieldProgressBar->SetPercent(DisplayShieldPercent);
        UpdateLabelPosition(ShieldLabelBox, ShieldProgressBar, DisplayShieldPercent, ShieldLabelX, InDeltaTime);
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

int32 UW_BossHP::ChooseTickStep(int32 MaxValue, int32 MaxSegments)
{
    // 1, 2, 5, 10, 20, 25, 50, 100... 순서로 올라가며 칸 수가 MaxSegments 이하가 되는 첫 값을 고릅니다.
    static const int32 NiceSteps[] = { 1, 2, 5, 10, 20, 25, 50, 100, 200, 250, 500, 1000 };
    const int32 SafeMaxSegments = FMath::Max(2, MaxSegments);
    for (int32 Step : NiceSteps)
    {
        if (FMath::DivideAndRoundUp(MaxValue, Step) <= SafeMaxSegments)
        {
            return Step;
        }
    }
    return FMath::Max(1, MaxValue);
}

void UW_BossHP::RebuildTicks(UCanvasPanel* TickPanel, int32 MaxValue)
{
    if (!IsValid(TickPanel) || !WidgetTree)
    {
        return;
    }

    TickPanel->ClearChildren();
    if (MaxValue <= 1)
    {
        return;
    }

    const UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(TickPanel->Slot);
    const FVector2D PanelSize = PanelSlot ? PanelSlot->GetSize() : FVector2D::ZeroVector;
    if (PanelSize.X <= 0.f || PanelSize.Y <= 0.f)
    {
        return;
    }

    const int32 Step = ChooseTickStep(MaxValue, MaxTickSegments);
    for (int32 Value = Step; Value < MaxValue; Value += Step)
    {
        UImage* Tick = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
        if (!IsValid(Tick))
        {
            continue;
        }

        // 그림 없는 브러시는 단색 사각형으로 그려집니다.
        FSlateBrush TickBrush;
        TickBrush.DrawAs = ESlateBrushDrawType::Box;
        TickBrush.SetResourceObject(nullptr);
        Tick->SetBrush(TickBrush);
        Tick->SetColorAndOpacity(TickColor);
        Tick->SetVisibility(ESlateVisibility::HitTestInvisible);

        const float X = PanelSize.X * static_cast<float>(Value) / static_cast<float>(MaxValue);
        if (UCanvasPanelSlot* TickSlot = TickPanel->AddChildToCanvas(Tick))
        {
            TickSlot->SetAnchors(FAnchors(0.f, 0.f));
            TickSlot->SetAlignment(FVector2D::ZeroVector);
            TickSlot->SetPosition(FVector2D(X - TickWidth * 0.5f, 0.f));
            TickSlot->SetSize(FVector2D(TickWidth, PanelSize.Y));
        }
    }
}

void UW_BossHP::UpdateLabelPosition(UWidget* LabelBox, const UProgressBar* Bar, float Percent, float& InOutLabelX, float InDeltaTime)
{
    if (!IsValid(LabelBox) || !IsValid(Bar))
    {
        return;
    }

    UCanvasPanelSlot* LabelSlot = Cast<UCanvasPanelSlot>(LabelBox->Slot);
    const UCanvasPanelSlot* BarSlot = Cast<UCanvasPanelSlot>(Bar->Slot);
    if (!LabelSlot || !BarSlot)
    {
        return;
    }

    FVector2D LabelSize = LabelBox->GetDesiredSize();
    if (LabelSize.X <= 0.f)
    {
        LabelBox->ForceLayoutPrepass();
        LabelSize = LabelBox->GetDesiredSize();
    }

    const FVector2D BarPos = BarSlot->GetPosition();
    const FVector2D BarSize = BarSlot->GetSize();
    const float FillEnd = BarPos.X + BarSize.X * FMath::Clamp(Percent, 0.f, 1.f);
    const float EmptyWidth = BarSize.X - (FillEnd - BarPos.X);

    // 빈 칸이 숫자 판보다 넓으면 게이지 끝 바로 바깥, 아니면 게이지 안쪽 끝에 붙입니다.
    float TargetX = (EmptyWidth >= LabelSize.X + LabelMargin * 2.f)
        ? FillEnd + LabelMargin
        : FillEnd - LabelSize.X - LabelMargin;
    TargetX = FMath::Clamp(TargetX, BarPos.X + LabelMargin, BarPos.X + BarSize.X - LabelSize.X - LabelMargin);

    InOutLabelX = (InOutLabelX < 0.f) ? TargetX : FMath::FInterpTo(InOutLabelX, TargetX, InDeltaTime, LabelFollowSpeed);
    LabelSlot->SetPosition(FVector2D(InOutLabelX, BarPos.Y + (BarSize.Y - LabelSize.Y) * 0.5f));
}

void UW_BossHP::UpdateBossWindow(const FBossHUDData& InData)
{
    if (!IsValid(BossEmblemImage))
    {
        return;
    }

    // 초상화(실시간 카메라)가 있으면 그것을, 없으면 문양을 둥근 창에 그립니다. 같은 그림이면 다시 묶지 않습니다.
    if (IsValid(InData.PortraitTexture) && IsValid(PortraitMaterial))
    {
        if (BoundPortraitTexture != InData.PortraitTexture)
        {
            ApplyBossPortrait(InData.PortraitTexture);
        }
        return;
    }

    if (IsValid(InData.BossIcon))
    {
        if (BoundEmblemTexture != InData.BossIcon || BoundPortraitTexture)
        {
            ApplyBossEmblem(InData.BossIcon);
        }
        return;
    }

    if (!BoundPortraitTexture && !BoundEmblemTexture)
    {
        BossEmblemImage->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UW_BossHP::ApplyBossPortrait(UTextureRenderTarget2D* Portrait)
{
    BossEmblemImage->SetBrushFromMaterial(PortraitMaterial);
    EmblemMaterialInstance = BossEmblemImage->GetDynamicMaterial();
    if (IsValid(EmblemMaterialInstance))
    {
        EmblemMaterialInstance->SetTextureParameterValue(FName("Portrait"), Portrait);
        EmblemMaterialInstance->SetScalarParameterValue(FName("Exposure"), PortraitExposure);
    }
    BossEmblemImage->SetVisibility(ESlateVisibility::HitTestInvisible);
    BoundPortraitTexture = Portrait;
    BoundEmblemTexture = nullptr;
}

void UW_BossHP::ApplyBossEmblem(UTexture2D* Emblem)
{
    // ReadyCoinSlot의 무기 아이콘과 같은 방식: 브러시의 UI 머티리얼(MI_WCoinIcon)에 그림과 색만 넣습니다.
    if (IsValid(EmblemMaterial))
    {
        BossEmblemImage->SetBrushFromMaterial(EmblemMaterial);
        EmblemMaterialInstance = BossEmblemImage->GetDynamicMaterial();
    }
    else
    {
        EmblemMaterialInstance = nullptr;
    }

    if (IsValid(EmblemMaterialInstance))
    {
        EmblemMaterialInstance->SetTextureParameterValue(FName("Weapon_Icon"), Emblem);
        EmblemMaterialInstance->SetVectorParameterValue(FName("Weapon_Color"), EmblemTint);
    }
    else
    {
        // 머티리얼이 없는 브러시면 그림을 그대로 씁니다(검은 바탕이 보일 수 있음).
        BossEmblemImage->SetBrushFromTexture(Emblem, false);
    }
    BossEmblemImage->SetVisibility(ESlateVisibility::HitTestInvisible);
    BoundEmblemTexture = Emblem;
    BoundPortraitTexture = nullptr;
}
