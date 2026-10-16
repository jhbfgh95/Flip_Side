#include "UI/CardGoldAppliedInfoWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/CoreStyle.h"
#include "UI/CoinDescriptionRichTextDecorator.h"

UCardGoldAppliedInfoWidget::UCardGoldAppliedInfoWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AppliedTextStyle = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>(TEXT("NormalText"));
	AppliedTextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 18));
	AppliedTextStyle.SetColorAndOpacity(FSlateColor(FLinearColor::White));
}

void UCardGoldAppliedInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsValid(WidgetTree)) return;
	const UPanelWidget* EmptyRoot = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!IsValid(WidgetTree->RootWidget) || (IsValid(EmptyRoot) && EmptyRoot->GetChildrenCount() == 0))
	{
		UBorder* Background = WidgetTree->ConstructWidget<UBorder>();
		UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>();
		AppliedEffectTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AppliedEffectTitle"));
		AppliedEffectsText = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass(), TEXT("AppliedEffectsText"));
		if (!IsValid(Background) || !IsValid(Content) || !IsValid(AppliedEffectTitle) || !IsValid(AppliedEffectsText)) return;
		Background->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.05f, 1.f));
		Background->SetPadding(FMargin(16.f));
		if (UVerticalBoxSlot* TitleSlot = Content->AddChildToVerticalBox(AppliedEffectTitle); IsValid(TitleSlot))
			TitleSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));
		Content->AddChildToVerticalBox(AppliedEffectsText);
		Background->SetContent(Content);
		WidgetTree->RootWidget = Background;
	}
}

void UCardGoldAppliedInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (IsValid(AppliedEffectsText))
	{
		AppliedEffectsText->SetDefaultTextStyle(AppliedTextStyle);
		AppliedEffectsText->SetAutoWrapText(false);
		AppliedEffectsText->SetDecorators({ UCoinDescriptionRichTextDecorator::StaticClass() });
	}
	RefreshText();
}

void UCardGoldAppliedInfoWidget::SetAppliedEffects(const FCoinCardModifiers& InModifiers, bool bInHasAppliedEffects)
{
	AppliedModifiers = InModifiers;
	bHasAppliedEffects = bInHasAppliedEffects;
	RefreshText();
}

void UCardGoldAppliedInfoWidget::RefreshText()
{
	if (IsValid(AppliedEffectTitle))
	{
		AppliedEffectTitle->SetText(NSLOCTEXT("CardGoldAppliedInfo", "Title", "현재 적용된 효과"));
		FSlateFontInfo TitleFont = AppliedTextStyle.Font;
		TitleFont.TypefaceFontName = DetailedTypeface;
		AppliedEffectTitle->SetFont(TitleFont);
		AppliedEffectTitle->SetColorAndOpacity(AppliedTextStyle.ColorAndOpacity);
	}
	if (!IsValid(AppliedEffectsText)) return;
	if (!bHasAppliedEffects)
	{
		AppliedEffectsText->SetText(NSLOCTEXT("CardGoldAppliedInfo", "Inactive", "적용 중인 효과가 없습니다."));
		return;
	}
	const TCHAR* const Keys[] = { TEXT("STAT:AttackPower"), TEXT("STAT:WeaponPower"), TEXT("STAT:Count"),
		TEXT("STAT:AttackRange"), TEXT("STAT:AbilityRange") };
	const int32 Values[] = { AppliedModifiers.AttackAdd, AppliedModifiers.BehaviorAdd, AppliedModifiers.CountAdd,
		AppliedModifiers.RangeAdd, AppliedModifiers.AbilityRangeAdd };
	FString Text;
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Keys); ++Index)
	{
		if (!Text.IsEmpty()) Text += TEXT("\n");
		Text += FString::Printf(TEXT("<coin key=\"%s\" value=\"%+d\"/>"), Keys[Index], Values[Index]);
	}
	AppliedEffectsText->SetText(FText::FromString(Text));
	AppliedEffectsText->RefreshTextLayout();
}

TSharedPtr<SWidget> UCardGoldAppliedInfoWidget::CreateStatDisplay(const FTextRunInfo& Run, const FTextBlockStyle& Style)
{
	return UCoinDescriptionRichTextDecorator::CreateDetailedStatDisplay(this, Run, Style,
		InlineIconSize, IconValueSpacing, IconNameSpacing, DetailedTypeface);
}
