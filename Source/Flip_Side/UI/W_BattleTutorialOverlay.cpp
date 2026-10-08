#include "UI/W_BattleTutorialOverlay.h"
#include "UI/TutorialClickHintWidget.h"
#include "UI/TutorialDescriptionWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/MeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Rendering/DrawElements.h"
#include "UObject/ConstructorHelpers.h"

UW_BattleTutorialOverlay::UW_BattleTutorialOverlay(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ExplanationLayouts = {
		FBattleTutorialExplanationLayout(FVector2D(-116.f, -420.f)),
		FBattleTutorialExplanationLayout(FVector2D(-400.f, 240.f)),
		FBattleTutorialExplanationLayout(FVector2D(-160.f, 210.f)),
		FBattleTutorialExplanationLayout(FVector2D(-430.f, 50.f)),
		FBattleTutorialExplanationLayout(FVector2D(-355.f, 55.f)),
		FBattleTutorialExplanationLayout(FVector2D(-50.f, 25.f)),
		FBattleTutorialExplanationLayout(FVector2D(-365.f, 240.f))
	};
	ClickHintWidgetClass = UTutorialClickHintWidget::StaticClass();
	DescriptionWidgetClass = TSoftClassPtr<UTutorialDescriptionWidget>(FSoftObjectPath(
		TEXT("/Game/BattleTutorial/WBP_TutorialDescription.WBP_TutorialDescription_C")));
	static ConstructorHelpers::FObjectFinder<UDataTable> Styles(TEXT("/Game/UI/Fonts/DT_RichTextStyles"));
	RichTextStyleSet = Styles.Object;
}

void UW_BattleTutorialOverlay::PostLoad()
{
	Super::PostLoad();
	if (!ExplanationPositions.IsEmpty())
	{
		ExplanationLayouts.Reset(ExplanationPositions.Num());
		for (const FVector2D& Position : ExplanationPositions)
			ExplanationLayouts.Add(FBattleTutorialExplanationLayout(Position));
		ExplanationPositions.Reset();
	}
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
	AreaTargets.Reset();
	bTransitionBusy = false;
	bAdvancePending = false;
	bHasHighlight = false;
	const int32 Index = ExplanationLayouts.IsValidIndex(Step.ExplanationPositionIndex) ? Step.ExplanationPositionIndex : 0;
	const FBattleTutorialExplanationLayout Layout = ExplanationLayouts.IsValidIndex(Index)
		? ExplanationLayouts[Index] : FBattleTutorialExplanationLayout();
	if (IsValid(ExplanationWidget))
	{
		ExplanationWidget->SetDescriptionText(Step.Text);
		ExplanationWidget->SetDescriptionSize(Layout.Size);
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ExplanationWidget->Slot))
			CanvasSlot->SetPosition(Layout.Position);
	}
	if (IsValid(ClickHint))
		ClickHint->SetVisibility(Step.bRequireHighlightedAction && !Step.ActionId.ToString().StartsWith(TEXT("Hover"))
			? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	if (IsValid(ExplanationWidget)) ExplanationWidget->SetVisibility(ESlateVisibility::Visible);
}

void UW_BattleTutorialOverlay::SetWidgetTarget(UWidget* Widget)
{
	WidgetTarget = Widget;
	ActorTarget.Reset();
	AreaTargets.Reset();
}

void UW_BattleTutorialOverlay::SetActorTarget(AActor* Actor, const FVector2D& HoleSize)
{
	ActorTarget = Actor;
	WidgetTarget.Reset();
	ActorHoleSize = HoleSize;
	AreaTargets.Reset();
}

void UW_BattleTutorialOverlay::SetActorTargets(const TArray<AActor*>& Actors)
{
	WidgetTarget.Reset(); ActorTarget.Reset(); AreaTargets.Reset();
	for (AActor* Actor : Actors) if (IsValid(Actor)) AreaTargets.Add(Actor);
}

void UW_BattleTutorialOverlay::SetTransitionBusy(bool bBusy)
{
	bTransitionBusy = bBusy;
	if (IsValid(ExplanationWidget)) ExplanationWidget->SetVisibility(bBusy ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (IsValid(ClickHint)) ClickHint->SetVisibility(ESlateVisibility::Collapsed);
}

void UW_BattleTutorialOverlay::SetAdvancePending(bool bPending)
{
	bAdvancePending = bPending;
	if (bPending)
	{
		// 설명과 강조는 유지하면서 실제 대상에 중복 입력이 전달되지 않도록 막습니다.
		if (IsValid(HoleInput)) HoleInput->SetVisibility(ESlateVisibility::Visible);
		if (IsValid(ClickHint)) ClickHint->SetVisibility(ESlateVisibility::Collapsed);
	}
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
	else if (ActorTarget.IsValid() || !AreaTargets.IsEmpty())
	{
		Min = FVector2D(TNumericLimits<float>::Max(), TNumericLimits<float>::Max()); Max = -Min;
		const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
		auto IncludePoint = [&](const FVector& WorldPoint)
		{
			FVector2D Screen;
			if (!UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), WorldPoint, Screen, false)) return;
			const FVector2D P = Geometry.AbsoluteToLocal(ViewportGeometry.LocalToAbsolute(Screen));
			Min.X = FMath::Min(Min.X, P.X); Min.Y = FMath::Min(Min.Y, P.Y);
			Max.X = FMath::Max(Max.X, P.X); Max.Y = FMath::Max(Max.Y, P.Y); bHasHighlight = true;
		};
		TArray<TWeakObjectPtr<AActor>> Targets = AreaTargets;
		if (ActorTarget.IsValid()) Targets.Add(ActorTarget);
		for (const auto& Entry : Targets)
		{
			AActor* Actor = Entry.Get(); if (!IsValid(Actor) || Actor->IsHidden()) continue;
			TArray<UMeshComponent*> Meshes; Actor->GetComponents<UMeshComponent>(Meshes);
			for (UMeshComponent* Mesh : Meshes)
			{
				if (!IsValid(Mesh) || !Mesh->IsVisible() || Mesh->bHiddenInGame || Mesh->Bounds.BoxExtent.IsNearlyZero()) continue;
				// 피벗 대신 실제로 보이는 메쉬의 월드 바운드 8개 모서리를 투영합니다.
				for (int32 Corner = 0; Corner < 8; ++Corner)
					IncludePoint(Mesh->Bounds.Origin + Mesh->Bounds.BoxExtent * FVector(
						Corner & 1 ? 1.f : -1.f, Corner & 2 ? 1.f : -1.f, Corner & 4 ? 1.f : -1.f));
			}
		}
		if (AActor* Actor = ActorTarget.Get(); IsValid(Actor))
		{
			FVector2D Screen;
			if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), Actor->GetActorLocation(), Screen, false))
			{
				const FVector2D Center = Geometry.AbsoluteToLocal(ViewportGeometry.LocalToAbsolute(Screen));
				const FVector2D Half = View * ActorHoleSize * 0.5f;
				Min.X = FMath::Min(Min.X, Center.X - Half.X); Min.Y = FMath::Min(Min.Y, Center.Y - Half.Y);
				Max.X = FMath::Max(Max.X, Center.X + Half.X); Max.Y = FMath::Max(Max.Y, Center.Y + Half.Y); bHasHighlight = true;
			}
		}
	}
	if (bHasHighlight)
	{
		const FVector2D Required = Max - Min + CurrentStep.Padding * 2.f;
		// 음수 여백으로 줄이되 사각형이 뒤집히지 않도록 중심을 유지하고 최소 크기를 보장합니다.
		HighlightSize = FVector2D(FMath::Max(1.f, Required.X), FMath::Max(1.f, Required.Y));
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
	HoleInput->SetVisibility(CurrentStep.bRequireHighlightedAction && !bTransitionBusy && !bAdvancePending && bHasHighlight
		? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (IsValid(ClickHint))
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ClickHint->Slot))
			CanvasSlot->SetPosition(FVector2D((Left + Right) * 0.5f, Top) + ClickHintOffset);
}

FReply UW_BattleTutorialOverlay::NativeOnPreviewMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
	if (bAdvancePending) return FReply::Handled();
	// 설명 BP의 Button 등이 클릭을 소비하기 전에 일반 설명 단계의 진행 입력을 받습니다.
	if (Event.GetEffectingButton() == EKeys::LeftMouseButton && !CurrentStep.bRequireHighlightedAction && !bTransitionBusy)
	{
		OnBattleTutorialOverlayClicked.Broadcast();
		return FReply::Handled();
	}
	return Super::NativeOnPreviewMouseButtonDown(Geometry, Event);
}

FReply UW_BattleTutorialOverlay::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
	if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (!CurrentStep.bRequireHighlightedAction && !bTransitionBusy && !bAdvancePending) OnBattleTutorialOverlayClicked.Broadcast();
		return FReply::Handled();
	}
	return FReply::Handled();
}

int32 UW_BattleTutorialOverlay::NativePaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullingRect,
	FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bParentEnabled) const
{
	const int32 Painted = Super::NativePaint(Args, Geometry, CullingRect, Elements, Layer, Style, bParentEnabled);
	if (!bHasHighlight || FrameThickness <= 0.f) return Painted;
	TArray<FVector2D> Points = {HighlightPosition, HighlightPosition + FVector2D(HighlightSize.X, 0.f),
		HighlightPosition + HighlightSize, HighlightPosition + FVector2D(0.f, HighlightSize.Y), HighlightPosition};
	FSlateDrawElement::MakeLines(Elements, Painted + 1, Geometry.ToPaintGeometry(), Points,
		ESlateDrawEffect::None, FrameColor, true, FrameThickness);
	return Painted + 1;
}
