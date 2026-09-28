#include "UI/W_CoinHPWidget.h"
#include "Actors/Component_Status.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

void UW_CoinHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MID = IsValid(HpImage) ? HpImage->GetDynamicMaterial() : nullptr;
	ShieldMID = IsValid(ShieldImage) ? ShieldImage->GetDynamicMaterial() : nullptr;
	if (StatusSource.IsValid())
	{
		InitHpWidget(StatusSource->GetMaxHP(), StatusSource->GetHP(), StatusSource->GetShield());
	}
	else
	{
		RefreshTextAndMaterialSettings();
		SetHpProgressBar(DisplayedHpPercent);
		SetShieldProgressBar(DisplayedShieldPercent);
	}
}

void UW_CoinHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bIsShieldAnimating)
	{
		ShieldAnimTime += FMath::Max(0.0f, InDeltaTime);
		const float ShieldAlpha = Duration > KINDA_SMALL_NUMBER
			? FMath::Clamp(ShieldAnimTime / Duration, 0.0f, 1.0f) : 1.0f;
		if (ShieldAlpha >= 1.0f) bIsShieldAnimating = false;
		SetShieldProgressBar(FMath::Lerp(StartShieldPercent, TargetShieldPercent, ShieldAlpha));
	}
	if (!bIsHpAnimating) return;
	AnimTime += FMath::Max(0.0f, InDeltaTime);
	const float Alpha = Duration > KINDA_SMALL_NUMBER
		? FMath::Clamp(AnimTime / Duration, 0.0f, 1.0f) : 1.0f;
	SetHpProgressBar(FMath::Lerp(StartHpPercent, TargetHpPercent, Alpha));
	if (Alpha >= 1.0f) bIsHpAnimating = false;
}

void UW_CoinHPWidget::InitializeWithStatus(UComponent_Status* InStatus)
{
	StatusSource = InStatus;
	if (StatusSource.IsValid())
		InitHpWidget(InStatus->GetMaxHP(), InStatus->GetHP(), InStatus->GetShield());
}

void UW_CoinHPWidget::InitHpWidget(int32 MaxHpValue, int32 CurrentHpValue, int32 ShieldValue)
{
	MaxHp = FMath::Max(1, MaxHpValue);
	CurrentHp = FMath::Clamp(CurrentHpValue, 0, MaxHp);
	CurrentShield = FMath::Max(0, ShieldValue);
	ShieldCapacity = StatusSource.IsValid()
		? FMath::Max(CurrentShield, StatusSource->GetShieldGaugeCapacity()) : CurrentShield;
	bIsShieldAnimating = false;
	TargetShieldPercent = ShieldCapacity > 0 ? static_cast<float>(CurrentShield) / ShieldCapacity : 0.0f;
	bIsHpAnimating = false;
	TargetHpPercent = static_cast<float>(CurrentHp) / MaxHp;
	RefreshTextAndMaterialSettings();
	SetHpProgressBar(TargetHpPercent);
	SetShieldProgressBar(TargetShieldPercent);
}

void UW_CoinHPWidget::RefreshFromStatus()
{
	// 최대 HP 변경과 현재 HP 변경이 연이어 전달되어도 차분을 중복 적용하지 않습니다.
	if (!StatusSource.IsValid()) return;
	MaxHp = FMath::Max(1, StatusSource->GetMaxHP());
	CurrentHp = FMath::Clamp(StatusSource->GetHP(), 0, MaxHp);
	CurrentShield = FMath::Max(0, StatusSource->GetShield());
	// 소진 시에도 마지막 칸 간격은 유지하여 0까지 부드럽게 비웁니다.
	if (CurrentShield > 0)
		ShieldCapacity = FMath::Max(CurrentShield, StatusSource->GetShieldGaugeCapacity());
}

void UW_CoinHPWidget::ChangeMaxHp(int32 HPModifier)
{
	if (StatusSource.IsValid()) RefreshFromStatus();
	else MaxHp = static_cast<int32>(FMath::Clamp<int64>(static_cast<int64>(MaxHp) + HPModifier, 1, MAX_int32));
	RefreshTextAndMaterialSettings();
	StartHpAnimation();
}

void UW_CoinHPWidget::ChangeCurrentHp(int32 HPModifier)
{
	if (StatusSource.IsValid()) RefreshFromStatus();
	else CurrentHp = static_cast<int32>(FMath::Clamp<int64>(static_cast<int64>(CurrentHp) + HPModifier, 0, MaxHp));
	RefreshTextAndMaterialSettings();
	StartHpAnimation();
}

void UW_CoinHPWidget::ChangeShield(int32 ShieldModifier)
{
	if (StatusSource.IsValid()) RefreshFromStatus();
	else
	{
		CurrentShield = static_cast<int32>(FMath::Clamp<int64>(static_cast<int64>(CurrentShield) + ShieldModifier, 0, MAX_int32));
		if (ShieldModifier > 0) ShieldCapacity = CurrentShield;
	}
	RefreshTextAndMaterialSettings();
	StartShieldAnimation();
}

void UW_CoinHPWidget::RefreshTextAndMaterialSettings()
{
	const float Ratio = FMath::Clamp(static_cast<float>(CurrentHp) / MaxHp, 0.0f, 1.0f);
	if (IsValid(CoinCurrentHPText))
	{
		CoinCurrentHPText->SetText(FText::AsNumber(CurrentHp));
		// 보호막 수치를 표시하는 동안 현재 HP 숫자는 자리를 유지한 채 숨깁니다.
		CoinCurrentHPText->SetVisibility(CurrentShield > 0
			? ESlateVisibility::Hidden : ESlateVisibility::HitTestInvisible);
		CoinCurrentHPText->SetColorAndOpacity(FSlateColor(Ratio <= 0.4f ? CurrentHPLowColor
			: Ratio <= 0.7f ? CurrentHPMediumColor : CurrentHPHighColor));
	}
	if (IsValid(CoinMaxHPText)) CoinMaxHPText->SetText(FText::AsNumber(MaxHp));
	const ESlateVisibility ShieldVisibility = CurrentShield > 0
		? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden;
	if (IsValid(ShieldText))
	{
		ShieldText->SetText(FText::AsNumber(CurrentShield));
		ShieldText->SetVisibility(ShieldVisibility);
	}
	if (IsValid(ShieldIcon)) ShieldIcon->SetVisibility(ShieldVisibility);
	if (IsValid(MID))
	{
		// 공통 UI 머테리얼: 0=아래부터 채우기, 1=왼쪽부터 소실되는 칸형 바.
		MID->SetScalarParameterValue(TEXT("FillMode"), UsesSegmentedBar() ? 1.0f : 0.0f);
		MID->SetScalarParameterValue(TEXT("SegmentCount"), static_cast<float>(MaxHp));
	}
	if (IsValid(ShieldMID))
	{
		ShieldMID->SetScalarParameterValue(TEXT("FillMode"), UsesSegmentedBar() ? 1.0f : 0.0f);
		ShieldMID->SetScalarParameterValue(TEXT("SegmentCount"), static_cast<float>(FMath::Max(1, ShieldCapacity)));
	}
}

void UW_CoinHPWidget::StartShieldAnimation()
{
	StartShieldPercent = DisplayedShieldPercent;
	TargetShieldPercent = ShieldCapacity > 0
		? FMath::Clamp(static_cast<float>(CurrentShield) / ShieldCapacity, 0.0f, 1.0f) : 0.0f;
	ShieldAnimTime = 0.0f;
	bIsShieldAnimating = Duration > KINDA_SMALL_NUMBER &&
		!FMath::IsNearlyEqual(StartShieldPercent, TargetShieldPercent);
	SetShieldProgressBar(bIsShieldAnimating ? DisplayedShieldPercent : TargetShieldPercent);
}

void UW_CoinHPWidget::SetShieldProgressBar(float Percentage)
{
	DisplayedShieldPercent = FMath::Clamp(Percentage, 0.0f, 1.0f);
	if (IsValid(ShieldMID))
	{
		ShieldMID->SetScalarParameterValue(TEXT("FillAmount"), DisplayedShieldPercent);
		ShieldMID->SetScalarParameterValue(TEXT("HpPercent"), DisplayedShieldPercent);
	}
	if (IsValid(ShieldImage))
	{
		const bool bVisible = CurrentShield > 0 || bIsShieldAnimating;
		ShieldImage->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UW_CoinHPWidget::StartHpAnimation()
{
	// 연속 피격/회복 시 현재 화면에 표시된 비율부터 이어서 움직입니다.
	StartHpPercent = DisplayedHpPercent;
	TargetHpPercent = FMath::Clamp(static_cast<float>(CurrentHp) / MaxHp, 0.0f, 1.0f);
	AnimTime = 0.0f;
	bIsHpAnimating = Duration > KINDA_SMALL_NUMBER && !FMath::IsNearlyEqual(StartHpPercent, TargetHpPercent);
	if (!bIsHpAnimating) SetHpProgressBar(TargetHpPercent);
}

void UW_CoinHPWidget::SetHpProgressBar(float Percentage)
{
	DisplayedHpPercent = FMath::Clamp(Percentage, 0.0f, 1.0f);
	if (IsValid(MID))
	{
		MID->SetScalarParameterValue(TEXT("FillAmount"), DisplayedHpPercent);
		// 기존 WBP 머테리얼과 호환되는 파라미터도 유지합니다.
		MID->SetScalarParameterValue(TEXT("HpPercent"), DisplayedHpPercent);
	}
}
