#include "UI/W_ShopTutorialWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

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
	if (IsValid(DialogueText))
	{
		DialogueText->SetText(InText);
	}
}

void UW_ShopTutorialWidget::SetDimMaskHole(FVector2D InAbsolutePosition, FVector2D InAbsoluteSize)
{
	const FGeometry& MaskGeometry = GetCachedGeometry();
	const FVector2D MaskSize = MaskGeometry.GetLocalSize();
	if (MaskSize.X <= 0.0f || MaskSize.Y <= 0.0f)
	{
		return;
	}

	const FVector2D LocalTopLeft = MaskGeometry.AbsoluteToLocal(InAbsolutePosition);
	const FVector2D LocalBottomRight = MaskGeometry.AbsoluteToLocal(InAbsolutePosition + InAbsoluteSize);
	const FVector2D LocalSize = LocalBottomRight - LocalTopLeft;
	const FVector2D LocalCenter = LocalTopLeft + LocalSize * 0.5f;

	if (IsValid(DimMaskMID))
	{
		DimMaskMID->SetScalarParameterValue(TEXT("HoleCenterX"), LocalCenter.X / MaskSize.X);
		DimMaskMID->SetScalarParameterValue(TEXT("HoleCenterY"), LocalCenter.Y / MaskSize.Y);
		DimMaskMID->SetScalarParameterValue(TEXT("HoleSizeX"), LocalSize.X / MaskSize.X);
		DimMaskMID->SetScalarParameterValue(TEXT("HoleSizeY"), LocalSize.Y / MaskSize.Y);
		DimMaskMID->SetScalarParameterValue(TEXT("UseHole"), 1.0f);
	}

}

void UW_ShopTutorialWidget::SetHighlightOverlayVisible(bool bIsHighlightVisible)
{
	const ESlateVisibility VisibilityState = bIsHighlightVisible
		? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Hidden;

	if (IsValid(DimMaskImage))
	{
		DimMaskImage->SetVisibility(VisibilityState);
	}

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
