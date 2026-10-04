#include "UI/TutorialClickHintWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/PanelWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"

UTutorialClickHintWidget::UTutorialClickHintWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> Icon(TEXT("/Game/UI/Cursor/cursor_click"));
	IconTexture = Icon.Object;
}

void UTutorialClickHintWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// BP에 디자인이 있으면 그대로 사용합니다.
	const UPanelWidget* EmptyRoot = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!WidgetTree->RootWidget || (IsValid(EmptyRoot) && EmptyRoot->GetChildrenCount() == 0))
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>();
		MouseIcon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("MouseIcon"));
		MouseIcon->SetDesiredSizeOverride(FVector2D(32.f, 32.f));
		ClickPrompt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ClickPrompt"));
		Row->AddChildToHorizontalBox(MouseIcon);
		Row->AddChildToHorizontalBox(ClickPrompt);
		WidgetTree->RootWidget = Row;
	}
}

void UTutorialClickHintWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (IsValid(ClickPrompt))
	{
		ClickPrompt->SetText(Prompt);
		ClickPrompt->SetColorAndOpacity(PromptColor);
	}
	if (IsValid(MouseIcon) && IsValid(IconTexture)) MouseIcon->SetBrushFromTexture(IconTexture);
}
