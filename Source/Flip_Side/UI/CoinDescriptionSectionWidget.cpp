#include "UI/CoinDescriptionSectionWidget.h"
#include "UI/CoinDescriptionFormatter.h"
#include "UI/CoinDescriptionRichTextDecorator.h"
#include "Components/RichTextBlock.h"
#include "Engine/Texture2D.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

TArray<FString> UCoinDescriptionSectionWidget::GetInlineStyleKeys() const
{
	return { TEXT("KW:Attack"), TEXT("KW:Mobility"), TEXT("KW:Hit"),
		TEXT("KW:Continuous"), TEXT("KW:Absorb"), TEXT("KW:Strike"),
		TEXT("STAT:AttackPower"), TEXT("STAT:WeaponPower"), TEXT("STAT:Count"),
		TEXT("BUFF:Absorb"), TEXT("BUFF:Strike"), TEXT("STAT:AttackRange"), TEXT("STAT:AbilityRange") };
}

void UCoinDescriptionSectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	for (URichTextBlock* Text : { HeaderRichText.Get(), DescriptionRichText.Get() })
	{
		if (!IsValid(Text)) continue;
		Text->SetDefaultColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetDecorators({ UCoinDescriptionRichTextDecorator::StaticClass() });
	}
	RefreshText();
}

void UCoinDescriptionSectionWidget::SetSectionData(const FCoinDescriptionSectionData& InData)
{
	SectionData = InData;
	RefreshText();
	OnSectionDataChanged(SectionData);
}

void UCoinDescriptionSectionWidget::SetDetailed(bool bInDetailed)
{
	if (bDetailed == bInDetailed) return;
	bDetailed = bInDetailed;
	// 선택한 설명은 그대로 두고, Shift 해제 시에도 문장 서식을 다시 계산합니다.
	RefreshText();
}

void UCoinDescriptionSectionWidget::RefreshText()
{
	if (IsValid(HeaderRichText))
	{
		FString Header;
		auto AppendKeyword = [&Header](FName Code)
		{
			if (Code.IsNone()) return;
			if (!Header.IsEmpty()) Header += TEXT(" ");
			Header += TEXT("[KW:") + Code.ToString() + TEXT("]");
		};
		AppendKeyword(SectionData.MainKeywordCode);
		for (FName Code : SectionData.AdditionalKeywordCodes) AppendKeyword(Code);
		FString Suffix = SectionData.HeaderSuffix.TrimStartAndEnd();
		Suffix.RemoveFromStart(TEXT(":"));
		Suffix.TrimStartInline();
		if (!Suffix.IsEmpty()) Header += (Header.IsEmpty() ? TEXT("") : TEXT(" ")) + Suffix;
		HeaderRichText->SetText(FText::FromString(FCoinDescriptionFormatter::ToRichText(Header)));
		HeaderRichText->RefreshTextLayout();
	}
	if (IsValid(DescriptionRichText))
	{
		DescriptionRichText->SetText(FText::FromString(FCoinDescriptionFormatter::ToRichText(SectionData.Description.ToString())));
		DescriptionRichText->RefreshTextLayout();
	}
}

TSharedPtr<SWidget> UCoinDescriptionSectionWidget::CreateInlineDisplay(FName Key, const FTextBlockStyle& Style)
{
	FCoinDescriptionTokenData Data;
	if (!FCoinDescriptionFormatter::ResolveToken(Key, SectionData, Data)) return nullptr;
	const FCoinDescriptionInlineStyle* DisplayStyle = InlineStyles.Find(Key);
	const FCoinDescriptionInlineParts Parts = FCoinDescriptionFormatter::BuildInlineParts(Data, bDetailed,
		!DisplayStyle || DisplayStyle->bShowValue);
	FSlateFontInfo Font = Style.Font;
	if (bDetailed) Font.TypefaceFontName = DetailedTypeface;
	const FSlateColor Color(bDetailed && DisplayStyle ? DisplayStyle->DetailedTextColor : FLinearColor::White);
	TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox).Visibility(EVisibility::HitTestInvisible);
	auto AddText = [&Row, &Font, &Color](const FString& Text)
	{
		if (Text.IsEmpty()) return;
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
			[SNew(STextBlock).Text(FText::FromString(Text)).Font(Font).ColorAndOpacity(Color)];
	};
	AddText(Parts.BeforeIcon);
	UTexture2D* Texture = DisplayStyle ? DisplayStyle->IconTexture.Get() : nullptr;
	if (IsValid(Texture))
	{
		// Brush는 Slate 이미지가 소유하고 Texture는 WBP의 InlineStyles가 GC로부터 보호합니다.
		TSharedRef<FSlateBrush> Brush = MakeShared<FSlateBrush>();
		Brush->SetResourceObject(Texture);
		Brush->DrawAs = ESlateBrushDrawType::Image;
		const FVector2D IconSize(FMath::Max(1.0, InlineIconSize.X), FMath::Max(1.0, InlineIconSize.Y));
		Brush->ImageSize = IconSize;
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
			[SNew(SBox).WidthOverride(IconSize.X).HeightOverride(IconSize.Y)
				[SNew(SImage).Image_Lambda([Brush]() -> const FSlateBrush* { return &Brush.Get(); })]];
	}
	else
	{
		// 미등록 아이콘은 잘못된 다른 아이콘으로 대체하지 않고 누락을 표시합니다.
		AddText(TEXT("?"));
	}
	AddText(Parts.AfterIcon);
	return Row;
}
