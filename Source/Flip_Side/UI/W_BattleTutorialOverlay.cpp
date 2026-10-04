#include "UI/W_BattleTutorialOverlay.h"
#include "UI/TutorialClickHintWidget.h"
#include "UI/TutorialDescriptionWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Rendering/DrawElements.h"
#include "UObject/ConstructorHelpers.h"

UW_BattleTutorialOverlay::UW_BattleTutorialOverlay(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ClickHintWidgetClass = UTutorialClickHintWidget::StaticClass();
	DescriptionWidgetClass = TSoftClassPtr<UTutorialDescriptionWidget>(FSoftObjectPath(
		TEXT("/Game/BattleTutorial/WBP_TutorialDescription.WBP_TutorialDescription_C")));
	static ConstructorHelpers::FObjectFinder<UDataTable> Styles(TEXT("/Game/UI/Fonts/DT_RichTextStyles"));
	RichTextStyleSet = Styles.Object;
}

void UW_BattleTutorialOverlay::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	TutorialCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("TutorialCanvas"));
	WidgetTree->RootWidget = TutorialCanvas;
	TutorialCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	for (int32 Index = 0; Index < 4; ++Index)
	{
		UBorder* Region = WidgetTree->ConstructWidget<UBorder>();
		Region->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, DimOpacity));
		TutorialCanvas->AddChildToCanvas(Region);
		DimRegions.Add(Region);
	}
	HoleInput = WidgetTree->ConstructWidget<UBorder>();
	HoleInput->SetBrushColor(FLinearColor::Transparent);
	TutorialCanvas->AddChildToCanvas(HoleInput);
	TSubclassOf<UTutorialDescriptionWidget> DescriptionClass = DescriptionWidgetClass.LoadSynchronous();
	if (!DescriptionClass) DescriptionClass = UTutorialDescriptionWidget::StaticClass();
	ExplanationWidget = CreateWidget<UTutorialDescriptionWidget>(GetOwningPlayer(), DescriptionClass);
	if (IsValid(ExplanationWidget))
	{
		ExplanationWidget->ConfigureFallbackAppearance(ExplanationWidth, RichTextStyleSet);
		UCanvasPanelSlot* TextSlot = TutorialCanvas->AddChildToCanvas(ExplanationWidget);
		TextSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		TextSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		TextSlot->SetAutoSize(true);
		TextSlot->SetZOrder(10);
	}
	if (ClickHintWidgetClass)
	{
		ClickHint = CreateWidget<UTutorialClickHintWidget>(GetOwningPlayer(), ClickHintWidgetClass);
		if (IsValid(ClickHint))
		{
			UCanvasPanelSlot* HintSlot = TutorialCanvas->AddChildToCanvas(ClickHint);
			HintSlot->SetAutoSize(true);
			HintSlot->SetAlignment(FVector2D(0.5f, 1.f));
			HintSlot->SetZOrder(11);
			ClickHint->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UW_BattleTutorialOverlay::ShowStep(const FBattleTutorialStep& Step)
{
	CurrentStep = Step;
	WidgetTarget.Reset();
	ActorTarget.Reset();
	bHasHighlight = false;
	const int32 Index = ExplanationPositions.IsValidIndex(Step.ExplanationPositionIndex) ? Step.ExplanationPositionIndex : 0;
	if (IsValid(ExplanationWidget))
	{
		ExplanationWidget->SetDescriptionText(Step.Text);
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ExplanationWidget->Slot))
			CanvasSlot->SetPosition(ExplanationPositions.IsValidIndex(Index) ? ExplanationPositions[Index] : FVector2D::ZeroVector);
	}
	if (IsValid(ClickHint))
		ClickHint->SetVisibility(Step.bRequireHighlightedAction ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UW_BattleTutorialOverlay::SetWidgetTarget(UWidget* Widget)
{
	WidgetTarget = Widget;
	ActorTarget.Reset();
}

void UW_BattleTutorialOverlay::SetActorTarget(AActor* Actor, const FVector2D& HoleSize)
{
	ActorTarget = Actor;
	WidgetTarget.Reset();
	ActorHoleSize = HoleSize;
}

void UW_BattleTutorialOverlay::Place(UWidget* Widget, const FVector2D& Position, const FVector2D& Size)
{
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
	{
		CanvasSlot->SetPosition(Position);
		CanvasSlot->SetSize(FVector2D(FMath::Max(0.f, Size.X), FMath::Max(0.f, Size.Y)));
	}
}

void UW_BattleTutorialOverlay::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);
	const FVector2D View = Geometry.GetLocalSize();
	FVector2D Min, Max;
	bHasHighlight = false;
	if (UWidget* Target = WidgetTarget.Get(); IsValid(Target) && Target->IsVisible())
	{
		const FGeometry& TargetGeometry = Target->GetCachedGeometry();
		const FVector2D Size = TargetGeometry.GetLocalSize();
		if (Size.X > 0.f && Size.Y > 0.f)
		{
			Min = FVector2D(TNumericLimits<float>::Max(), TNumericLimits<float>::Max());
			Max = -Min;
			const FVector2D Corners[] = {FVector2D::ZeroVector, FVector2D(Size.X, 0.f), Size, FVector2D(0.f, Size.Y)};
			for (const FVector2D& Corner : Corners)
			{
				const FVector2D Point = Geometry.AbsoluteToLocal(TargetGeometry.LocalToAbsolute(Corner));
				Min.X = FMath::Min(Min.X, Point.X); Min.Y = FMath::Min(Min.Y, Point.Y);
				Max.X = FMath::Max(Max.X, Point.X); Max.Y = FMath::Max(Max.Y, Point.Y);
			}
			bHasHighlight = true;
		}
	}
	else if (AActor* TargetActor = ActorTarget.Get(); IsValid(TargetActor))
	{
		FVector2D Screen;
		if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), TargetActor->GetActorLocation(), Screen, false))
		{
			const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
			const FVector2D Center = Geometry.AbsoluteToLocal(ViewportGeometry.LocalToAbsolute(Screen));
			const FVector2D Half = View * ActorHoleSize * 0.5f;
			Min = Center - Half; Max = Center + Half;
			bHasHighlight = true;
		}
	}
	if (bHasHighlight)
	{
		const FVector2D HighlightPadding(FMath::Max(0.f, CurrentStep.Padding.X), FMath::Max(0.f, CurrentStep.Padding.Y));
		const FVector2D Required = Max - Min + HighlightPadding * 2.f;
		const float Ratios[] = {4.f / 3.f, 1.f, 3.f / 4.f};
		float BestArea = TNumericLimits<float>::Max();
		for (int32 Index = 0; Index < 3; ++Index)
		{
			if (CurrentStep.Frame != EBattleTutorialFrame::Auto && static_cast<int32>(CurrentStep.Frame) != Index + 1) continue;
			const float Width = FMath::Max(Required.X, Required.Y * Ratios[Index]);
			const FVector2D Size(Width, Width / Ratios[Index]);
			if (Size.X * Size.Y < BestArea)
			{
				BestArea = Size.X * Size.Y; HighlightSize = Size; FrameIndex = Index;
			}
		}
		HighlightPosition = (Min + Max - HighlightSize) * 0.5f;
	}
	const FVector2D A = bHasHighlight ? HighlightPosition.ClampAxes(0.f, TNumericLimits<float>::Max()) : FVector2D::ZeroVector;
	const FVector2D B = bHasHighlight ? HighlightPosition + HighlightSize : FVector2D::ZeroVector;
	const float Left = FMath::Clamp(A.X, 0.f, View.X), Top = FMath::Clamp(A.Y, 0.f, View.Y);
	const float Right = FMath::Clamp(B.X, Left, View.X), Bottom = FMath::Clamp(B.Y, Top, View.Y);
	Place(DimRegions[0], FVector2D::ZeroVector, FVector2D(View.X, Top));
	Place(DimRegions[1], FVector2D(0.f, Bottom), FVector2D(View.X, View.Y - Bottom));
	Place(DimRegions[2], FVector2D(0.f, Top), FVector2D(Left, Bottom - Top));
	Place(DimRegions[3], FVector2D(Right, Top), FVector2D(View.X - Right, Bottom - Top));
	Place(HoleInput, FVector2D(Left, Top), FVector2D(Right - Left, Bottom - Top));
	HoleInput->SetVisibility(CurrentStep.bRequireHighlightedAction ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (IsValid(ClickHint))
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ClickHint->Slot))
			CanvasSlot->SetPosition(FVector2D((Left + Right) * 0.5f, Top) + ClickHintOffset);
}

FReply UW_BattleTutorialOverlay::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
	if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (!CurrentStep.bRequireHighlightedAction) OnBattleTutorialOverlayClicked.Broadcast();
		return FReply::Handled();
	}
	return FReply::Handled();
}

int32 UW_BattleTutorialOverlay::NativePaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullingRect,
	FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bParentEnabled) const
{
	const int32 Painted = Super::NativePaint(Args, Geometry, CullingRect, Elements, Layer, Style, bParentEnabled);
	if (!bHasHighlight) return Painted;
	if (FrameBrushes.IsValidIndex(FrameIndex) && FrameBrushes[FrameIndex].GetResourceObject())
		FSlateDrawElement::MakeBox(Elements, Painted + 1,
			Geometry.ToPaintGeometry(HighlightSize, FSlateLayoutTransform(HighlightPosition)), &FrameBrushes[FrameIndex],
			ESlateDrawEffect::None, FrameColor);
	else
	{
		TArray<FVector2D> Points = {HighlightPosition, HighlightPosition + FVector2D(HighlightSize.X, 0.f),
			HighlightPosition + HighlightSize, HighlightPosition + FVector2D(0.f, HighlightSize.Y), HighlightPosition};
		FSlateDrawElement::MakeLines(Elements, Painted + 1, Geometry.ToPaintGeometry(), Points,
			ESlateDrawEffect::None, FrameColor, true, FrameThickness);
	}
	return Painted + 1;
}
