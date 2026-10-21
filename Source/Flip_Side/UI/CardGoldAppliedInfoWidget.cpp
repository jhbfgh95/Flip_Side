#include "UI/CardGoldAppliedInfoWidget.h"

#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "UI/CoinDescriptionRichTextDecorator.h"
#include "Widgets/Layout/SBox.h"

void UCardGoldAppliedInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (IsValid(AppliedEffectsText))
		AppliedEffectsText->SetDecorators({ UCoinDescriptionRichTextDecorator::StaticClass() });
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
		AppliedEffectTitle->SetText(NSLOCTEXT("CardGoldAppliedInfo", "Title", "현재 적용된 효과"));
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
		if (Index > 0 && Index % 2 == 0) Text += TEXT("\n");
		Text += FString::Printf(TEXT("<coin key=\"%s\" value=\"%+d\"%s/>"), Keys[Index], Values[Index],
			Index % 2 == 1 ? TEXT(" spaced=\"true\"") : TEXT(""));
	}
	AppliedEffectsText->SetText(FText::FromString(Text));
	AppliedEffectsText->RefreshTextLayout();
}

TSharedPtr<SWidget> UCardGoldAppliedInfoWidget::CreateStatDisplay(const FTextRunInfo& Run, const FTextBlockStyle& Style)
{
	TSharedPtr<SWidget> Display = UCoinDescriptionRichTextDecorator::CreateDetailedStatDisplay(this, Run, Style,
		InlineIconSize, IconValueSpacing, IconNameSpacing, Style.Font.TypefaceFontName);
	if (!Display.IsValid() || !Run.MetaData.Contains(TEXT("spaced"))) return Display;
	return SNew(SBox).Padding(FMargin(FMath::Max(0.f, StatSpacing), 0.f, 0.f, 0.f))
		[Display.ToSharedRef()];
}
