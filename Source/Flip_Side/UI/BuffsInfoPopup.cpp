#include "UI/BuffsInfoPopup.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/CoreStyle.h"
#include "UI/CoinDescriptionFormatter.h"
#include "UI/CoinDescriptionRichTextDecorator.h"

void UBuffsInfoPopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsValid(WidgetTree)) return;
	// BP 디자인이 없을 때도 HUD에서 바로 표시할 수 있는 기본 레이아웃입니다.
	if (!WidgetTree->RootWidget)
	{
		USizeBox* Size = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BuffInfoSizeBox"));
		Size->SetWidthOverride(380.f);
		UBorder* Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BuffInfoBackground"));
		Background->SetBrushColor(FLinearColor(0.035f, 0.035f, 0.045f, 0.98f));
		Background->SetPadding(FMargin(18.f));
		UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>();
		UHorizontalBox* Header = WidgetTree->ConstructWidget<UHorizontalBox>();
		USizeBox* IconSize = WidgetTree->ConstructWidget<USizeBox>();
		IconSize->SetWidthOverride(48.f);
		IconSize->SetHeightOverride(48.f);
		BuffIconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("BuffIconImage"));
		IconSize->SetContent(BuffIconImage);
		Header->AddChildToHorizontalBox(IconSize);
		BuffNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BuffNameText"));
		BuffNameText->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), 20));
		BuffNameText->SetAutoWrapText(true);
		UHorizontalBoxSlot* NameSlot = Header->AddChildToHorizontalBox(BuffNameText);
		NameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		NameSlot->SetVerticalAlignment(VAlign_Center);
		NameSlot->SetPadding(FMargin(12.f, 0.f, 0.f, 0.f));
		Content->AddChildToVerticalBox(Header);
		BuffDescriptionText = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass(), TEXT("BuffDescriptionText"));
		FTextBlockStyle TextStyle;
		TextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 16));
		TextStyle.SetColorAndOpacity(FSlateColor(FLinearColor::White));
		BuffDescriptionText->SetDefaultTextStyle(TextStyle);
		BuffDescriptionText->SetAutoWrapText(true);
		BuffDescriptionText->SetWrapTextAt(344.f);
		Content->AddChildToVerticalBox(BuffDescriptionText)->SetPadding(FMargin(0.f, 14.f, 0.f, 0.f));
		Background->SetContent(Content);
		Size->SetContent(Background);
		WidgetTree->RootWidget = Size;
	}
	if (IsValid(BuffDescriptionText))
		BuffDescriptionText->SetDecorators({ UCoinDescriptionRichTextDecorator::StaticClass() });
	SetVisibility(ESlateVisibility::Collapsed);
}

void UBuffsInfoPopup::SetBuffInfo(const FBattleStatusEffectViewData& InData)
{
	if (IsValid(BuffNameText)) BuffNameText->SetText(InData.DisplayName);
	if (IsValid(BuffIconImage))
	{
		BuffIconImage->SetBrushFromTexture(InData.Icon, false);
		BuffIconImage->SetVisibility(IsValid(InData.Icon) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (IsValid(BuffDescriptionText))
	{
		BuffDescriptionText->SetText(FText::FromString(FCoinDescriptionFormatter::ToRichText(InData.Description.ToString())));
		BuffDescriptionText->RefreshTextLayout();
	}
	// 팝업이 원래 아이콘의 호버를 가로채지 않도록 합니다.
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

TSharedPtr<SWidget> UBuffsInfoPopup::CreateStatDisplay(const FTextRunInfo& Run, const FTextBlockStyle& Style)
{
	return UCoinDescriptionRichTextDecorator::CreateDetailedStatDisplay(this, Run, Style,
		InlineIconSize, 4.f, IconNameSpacing, Style.Font.TypefaceFontName);
}
