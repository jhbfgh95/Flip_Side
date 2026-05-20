#include "UI/W_PoisonTimerUI.h"
#include "Components/Image.h"

void UW_PoisonTimerUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsActive) return;

	RemainingTime = FMath::Max(RemainingTime - InDeltaTime, 0.f);

	if (ClockHandImage)
	{
		const float Angle = (1.f - RemainingTime / TotalDuration) * 360.f;
		ClockHandImage->SetRenderTransformAngle(Angle);
	}
}

void UW_PoisonTimerUI::ShowPoisonTimer(float Duration)
{
	TotalDuration = Duration;
	RemainingTime = Duration;
	bIsActive = true;
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (ClockHandImage)
	{
		ClockHandImage->SetRenderTransformPivot(FVector2D(0.5f, 1.0f));
		ClockHandImage->SetRenderTransformAngle(0.f);
	}
}

void UW_PoisonTimerUI::ResetPoisonTimer()
{
	RemainingTime = TotalDuration;

	if (ClockHandImage)
		ClockHandImage->SetRenderTransformAngle(0.f);
}

void UW_PoisonTimerUI::HidePoisonTimer()
{
	bIsActive = false;
	SetVisibility(ESlateVisibility::Collapsed);
}
