#include "UI/TutorialDescriptionWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/SizeBox.h"
#include "Engine/DataTable.h"

void UTutorialDescriptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsValid(WidgetTree)) return;
	const UPanelWidget* EmptyRoot = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!WidgetTree->RootWidget || (IsValid(EmptyRoot) && EmptyRoot->GetChildrenCount() == 0))
	{
		DescriptionSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("DescriptionSizeBox"));
		DescriptionSizeBox->SetWidthOverride(620.f);
		DescriptionBackground = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DescriptionBackground"));
		DescriptionBackground->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.05f, 1.f));
		DescriptionBackground->SetPadding(FMargin(24.f));
		DescriptionText = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass(), TEXT("DescriptionText"));
		DescriptionText->SetAutoWrapText(true);
		DescriptionText->SetWrapTextAt(572.f);
		DescriptionBackground->SetContent(DescriptionText);
		DescriptionSizeBox->AddChild(DescriptionBackground);
		WidgetTree->RootWidget = DescriptionSizeBox;
		bUsingFallbackLayout = true;
	}
}

void UTutorialDescriptionWidget::SetDescriptionText(const FText& Text)
{
	if (!IsValid(DescriptionText)) return;
	FString Value = Text.ToString();
	Value.ReplaceInline(TEXT("\\n"), TEXT("\n"));
	DescriptionText->SetText(FText::FromString(Value));
}

void UTutorialDescriptionWidget::ConfigureFallbackAppearance(float Width, UDataTable* StyleSet)
{
	if (bUsingFallbackLayout)
	{
		if (IsValid(DescriptionSizeBox)) DescriptionSizeBox->SetWidthOverride(FMath::Max(100.f, Width));
		if (IsValid(DescriptionText)) DescriptionText->SetWrapTextAt(FMath::Max(100.f, Width - 48.f));
	}
	if (IsValid(DescriptionText) && !IsValid(DescriptionText->GetTextStyleSet()) && IsValid(StyleSet))
		DescriptionText->SetTextStyleSet(StyleSet);
}
