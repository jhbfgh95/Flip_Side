#include "UI/CoinInfoPinHintWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void UCoinInfoPinHintWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 안내 전체가 입력을 통과시켜 아래 월드의 호버와 클릭을 유지합니다.
	SetVisibility(ESlateVisibility::HitTestInvisible);
	SetAlignmentInViewport(FVector2D::ZeroVector);
}

void UCoinInfoPinHintWidget::UpdatePositionAtCursor()
{
	APlayerController* Controller = GetOwningPlayer();
	UWorld* World = GetWorld();
	if (!IsValid(Controller) || !IsValid(World)) return;
	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!Controller->GetMousePosition(MouseX, MouseY))
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	if (ViewportScale <= 0.0f) return;

	SetVisibility(ESlateVisibility::HitTestInvisible);
	ForceLayoutPrepass();
	const FVector2D HintSize = GetDesiredSize();

	int32 ViewportWidth = 0;
	int32 ViewportHeight = 0;
	Controller->GetViewportSize(ViewportWidth, ViewportHeight);
	const FVector2D ViewportSize = FVector2D(ViewportWidth, ViewportHeight) / ViewportScale;
	const FVector2D MousePosition = FVector2D(MouseX, MouseY) / ViewportScale;
	FVector2D Position = MousePosition + CursorOffset - FVector2D(0.0f, HintSize.Y * 0.5f);
	if (Position.X + HintSize.X > ViewportSize.X)
	{
		Position.X = MousePosition.X - CursorOffset.X - HintSize.X;
	}
	Position.X = FMath::Clamp(Position.X, 0.0, FMath::Max(0.0, ViewportSize.X - HintSize.X));
	Position.Y = FMath::Clamp(Position.Y, 0.0, FMath::Max(0.0, ViewportSize.Y - HintSize.Y));
	SetDesiredSizeInViewport(HintSize);
	SetPositionInViewport(Position, false);
}
