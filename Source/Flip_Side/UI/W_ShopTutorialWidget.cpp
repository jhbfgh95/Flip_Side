#include "UI/W_ShopTutorialWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/DrawElements.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/MeshComponent.h"
#include "GameFramework/Actor.h"

void UW_ShopTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(DialogueButton))
	{
		DialogueButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleDialogueClicked);
	}

	if (IsValid(DimMaskImage))
	{
		DimMaskMID = DimMaskImage->GetDynamicMaterial();
		UpdateDimMaskParameters();
	}
}

void UW_ShopTutorialWidget::SetTutorialActive(bool bIsActive)
{
	SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UW_ShopTutorialWidget::SetTutorialInputEnabled(bool bInEnabled)
{
	SetVisibility(bInEnabled ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);
}

void UW_ShopTutorialWidget::SetDialogueText(const FText& InText)
{
	if (IsValid(DialogueBorder))
	{
		DialogueBorder->SetVisibility(InText.IsEmpty()
			? ESlateVisibility::Collapsed
			: ESlateVisibility::SelfHitTestInvisible);
	}

	if (IsValid(DialogueText))
	{
		DialogueText->SetText(InText);
	}
}

void UW_ShopTutorialWidget::SetDialogueBorderVisible(bool bInVisible)
{
	if (IsValid(DialogueBorder))
	{
		DialogueBorder->SetVisibility(bInVisible
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	}
}

void UW_ShopTutorialWidget::SetDimMaskHole(FVector2D InAbsolutePosition, FVector2D InAbsoluteSize)
{
	bHasHighlight = false;
	const FGeometry& MaskGeometry = GetCachedGeometry();
	const FVector2D MaskSize = MaskGeometry.GetLocalSize();
	if (MaskSize.X <= 0.0f || MaskSize.Y <= 0.0f)
	{
		return;
	}

	const FVector2D LocalTopLeft = MaskGeometry.AbsoluteToLocal(InAbsolutePosition);
	const FVector2D LocalBottomRight = MaskGeometry.AbsoluteToLocal(InAbsolutePosition + InAbsoluteSize);
	const FVector2D LocalSize = LocalBottomRight - LocalTopLeft;
	if (LocalSize.X <= 0.f || LocalSize.Y <= 0.f)
	{
		return;
	}
	HighlightPosition = LocalTopLeft;
	HighlightSize = LocalSize;
	bHasHighlight = true;
	HighlightBoxes.Emplace(HighlightPosition, HighlightSize);

	UpdateDimMaskParameters();

}

void UW_ShopTutorialWidget::UpdateDimMaskParameters()
{
	if (!IsValid(DimMaskMID)) return;

	// 먼저 두 구멍을 비활성화한 뒤, 저장된 박스만 활성화합니다.
	DimMaskMID->SetScalarParameterValue(TEXT("UseHole"), 0.f);
	DimMaskMID->SetScalarParameterValue(TEXT("UseHole2"), 0.f);
	if (!bHighlightOverlayVisible) return;

	const FVector2D MaskSize = GetCachedGeometry().GetLocalSize();
	if (MaskSize.X <= 0.f || MaskSize.Y <= 0.f) return;

	const FName CenterXNames[] = {TEXT("HoleCenterX"), TEXT("Hole2CenterX")};
	const FName CenterYNames[] = {TEXT("HoleCenterY"), TEXT("Hole2CenterY")};
	const FName SizeXNames[] = {TEXT("HoleSizeX"), TEXT("Hole2SizeX")};
	const FName SizeYNames[] = {TEXT("HoleSizeY"), TEXT("Hole2SizeY")};
	const FName EnabledNames[] = {TEXT("UseHole"), TEXT("UseHole2")};
	for (int32 Index = 0; Index < FMath::Min(HighlightBoxes.Num(), 2); ++Index)
	{
		const FVector2D& Position = HighlightBoxes[Index].Key;
		const FVector2D& Size = HighlightBoxes[Index].Value;
		const FVector2D Center = Position + Size * 0.5f;
		DimMaskMID->SetScalarParameterValue(CenterXNames[Index], Center.X / MaskSize.X);
		DimMaskMID->SetScalarParameterValue(CenterYNames[Index], Center.Y / MaskSize.Y);
		DimMaskMID->SetScalarParameterValue(SizeXNames[Index], Size.X / MaskSize.X);
		DimMaskMID->SetScalarParameterValue(SizeYNames[Index], Size.Y / MaskSize.Y);
		DimMaskMID->SetScalarParameterValue(EnabledNames[Index], 1.f);
	}
}

void UW_ShopTutorialWidget::SetHighlightOverlayVisible(bool bIsHighlightVisible)
{
	bHighlightOverlayVisible = bIsHighlightVisible;
	if (!bIsHighlightVisible)
	{
		HighlightBoxes.Reset();
		bHasHighlight = false;
	}
	UpdateDimMaskParameters();
	const ESlateVisibility VisibilityState = bIsHighlightVisible
		? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Hidden;

	if (IsValid(DimMaskImage))
	{
		DimMaskImage->SetVisibility(VisibilityState);
	}

}

void UW_ShopTutorialWidget::SetHighlightTargetWidget(UWidget* InTargetWidget, bool bKeepExistingBoxes)
{
	if (!bKeepExistingBoxes) SetHighlightOverlayVisible(false);
	if (!IsValid(InTargetWidget)) return;
	SetHighlightOverlayVisible(true);
	UpdateHighlightGeometry(InTargetWidget, nullptr, FVector2D::ZeroVector);
}

void UW_ShopTutorialWidget::SetHighlightTargetActor(AActor* InTargetActor, FVector2D InMinimumScreenSize, bool bKeepExistingBoxes)
{
	if (!bKeepExistingBoxes) SetHighlightOverlayVisible(false);
	if (!IsValid(InTargetActor)) return;
	const FVector2D MinimumScreenSize(FMath::Max(0.f, InMinimumScreenSize.X),
		FMath::Max(0.f, InMinimumScreenSize.Y));
	SetHighlightOverlayVisible(true);
	UpdateHighlightGeometry(nullptr, InTargetActor, MinimumScreenSize);
}

void UW_ShopTutorialWidget::UpdateHighlightGeometry(UWidget* Target, AActor* Actor, FVector2D ActorMinimumScreenSize)
{
	const FGeometry& MyGeometry = GetCachedGeometry();
	if (!bHighlightOverlayVisible)
	{
		return;
	}
	if (IsValid(Actor))
	{
		bHasHighlight = false;
		if (Actor->IsHidden() || !IsValid(GetOwningPlayer())) return;
		const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
		FVector2D Min(TNumericLimits<float>::Max(), TNumericLimits<float>::Max());
		FVector2D Max = -Min;
		bool bHasProjectedPoint = false;
		TArray<UMeshComponent*> Meshes;
		Actor->GetComponents<UMeshComponent>(Meshes);
		for (UMeshComponent* Mesh : Meshes)
		{
			if (!IsValid(Mesh) || !Mesh->IsVisible() || Mesh->bHiddenInGame || Mesh->Bounds.BoxExtent.IsNearlyZero()) continue;
			for (int32 Corner = 0; Corner < 8; ++Corner)
			{
				const FVector WorldPoint = Mesh->Bounds.Origin + Mesh->Bounds.BoxExtent * FVector(
					Corner & 1 ? 1.f : -1.f, Corner & 2 ? 1.f : -1.f, Corner & 4 ? 1.f : -1.f);
				FVector2D Screen;
				if (!UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), WorldPoint, Screen, false)) continue;
				const FVector2D Point = MyGeometry.AbsoluteToLocal(ViewportGeometry.LocalToAbsolute(Screen));
				Min.X = FMath::Min(Min.X, Point.X); Min.Y = FMath::Min(Min.Y, Point.Y);
				Max.X = FMath::Max(Max.X, Point.X); Max.Y = FMath::Max(Max.Y, Point.Y);
				bHasProjectedPoint = true;
			}
		}
		FVector2D ScreenCenter;
		if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), Actor->GetActorLocation(), ScreenCenter, false))
		{
			const FVector2D Center = MyGeometry.AbsoluteToLocal(ViewportGeometry.LocalToAbsolute(ScreenCenter));
			const FVector2D Half = MyGeometry.GetLocalSize() * ActorMinimumScreenSize * 0.5f;
			Min.X = FMath::Min(Min.X, Center.X - Half.X); Min.Y = FMath::Min(Min.Y, Center.Y - Half.Y);
			Max.X = FMath::Max(Max.X, Center.X + Half.X); Max.Y = FMath::Max(Max.Y, Center.Y + Half.Y);
			bHasProjectedPoint = true;
		}
		if (bHasProjectedPoint)
		{
			const FVector2D AbsoluteMin = MyGeometry.LocalToAbsolute(Min);
			SetDimMaskHole(AbsoluteMin, MyGeometry.LocalToAbsolute(Max) - AbsoluteMin);
		}
		return;
	}
	if (!IsValid(Target) || !Target->IsVisible())
	{
		bHasHighlight = false;
		return;
	}
	const FGeometry& TargetGeometry = Target->GetCachedGeometry();
	const FVector2D Size = TargetGeometry.GetLocalSize();
	if (Size.X <= 0.f || Size.Y <= 0.f)
	{
		bHasHighlight = false;
		return;
	}
	const FVector2D Corners[] = {FVector2D::ZeroVector, FVector2D(Size.X, 0.f),
		Size, FVector2D(0.f, Size.Y)};
	FVector2D Min(TNumericLimits<float>::Max(), TNumericLimits<float>::Max());
	FVector2D Max = -Min;
	for (const FVector2D& Corner : Corners)
	{
		const FVector2D Point = TargetGeometry.LocalToAbsolute(Corner);
		Min.X = FMath::Min(Min.X, Point.X);
		Min.Y = FMath::Min(Min.Y, Point.Y);
		Max.X = FMath::Max(Max.X, Point.X);
		Max.Y = FMath::Max(Max.Y, Point.Y);
	}
	SetDimMaskHole(Min, Max - Min);
}

int32 UW_ShopTutorialWidget::NativePaint(const FPaintArgs& Args, const FGeometry& Geometry,
	const FSlateRect& CullingRect, FSlateWindowElementList& Elements, int32 Layer,
	const FWidgetStyle& Style, bool bParentEnabled) const
{
	const int32 Painted = Super::NativePaint(Args, Geometry, CullingRect, Elements, Layer, Style, bParentEnabled);
	if (!bHighlightOverlayVisible || HighlightBoxes.IsEmpty() || FrameThickness <= 0.f)
	{
		return Painted;
	}
	for (const TPair<FVector2D, FVector2D>& Box : HighlightBoxes)
	{
		const FVector2D& Position = Box.Key;
		const FVector2D& Size = Box.Value;
		const TArray<FVector2D> Points = {Position,
			Position + FVector2D(Size.X, 0.f), Position + Size,
			Position + FVector2D(0.f, Size.Y), Position};
		FSlateDrawElement::MakeLines(Elements, Painted + 1, Geometry.ToPaintGeometry(), Points,
			ESlateDrawEffect::None, FrameColor, true, FrameThickness);
	}
	return Painted + 1;
}

void UW_ShopTutorialWidget::SetDialogueBoxLayout(FVector2D InAnchorPosition, FVector2D InSize)
{
	if ((InAnchorPosition.X == -1.0f && InAnchorPosition.Y == -1.0f) ||
		(InSize.X == -1.0f && InSize.Y == -1.0f))
	{
		return;
	}

	if (UCanvasPanelSlot* DialogueBorderSlot = IsValid(DialogueBorder)
		? Cast<UCanvasPanelSlot>(DialogueBorder->Slot)
		: nullptr)
	{
		DialogueBorderSlot->SetAnchors(FAnchors(InAnchorPosition.X, InAnchorPosition.Y));
		DialogueBorderSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		DialogueBorderSlot->SetPosition(FVector2D::ZeroVector);
		DialogueBorderSlot->SetSize(InSize);
	}
}

void UW_ShopTutorialWidget::HandleDialogueClicked()
{
	OnDialogueClicked.Broadcast();
}
