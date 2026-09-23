#include "UI/CoinDescriptionSectionWidget.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "UI/CoinDescriptionFormatter.h"
#include "UI/CoinDescriptionExpression.h"
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
		FString Suffix = SectionData.HeaderSuffix.TrimStartAndEnd();
		Suffix.RemoveFromStart(TEXT(":"));
		Suffix.TrimStartAndEndInline();
		Suffix.RemoveFromEnd(TEXT("."));
		Suffix.TrimEndInline();
		if (Suffix.StartsWith(TEXT("<Stat>")) && Suffix.EndsWith(TEXT("</Stat>")))
			Suffix = Suffix.Mid(6, Suffix.Len() - 13).TrimStartAndEnd();
		const bool bContinuousStat = SectionData.AdditionalKeywordCodes.Contains(FName(TEXT("Continuous")))
			&& (Suffix == TEXT("[STAT:Count]") || Suffix == TEXT("[STAT:WeaponPower]"));
		auto AppendKeyword = [&Header](FName Code)
		{
			if (Code.IsNone()) return;
			if (!Header.IsEmpty()) Header += TEXT(" ");
			Header += TEXT("[KW:") + Code.ToString() + TEXT("]");
		};
		AppendKeyword(SectionData.MainKeywordCode);
		for (FName Code : SectionData.AdditionalKeywordCodes) AppendKeyword(Code);
		if (!bContinuousStat && !Suffix.IsEmpty()) Header += (Header.IsEmpty() ? TEXT("") : TEXT(" ")) + Suffix;
		FString RichHeader = FCoinDescriptionFormatter::ToRichText(Header);
		// 헤더의 모든 토큰은 Shift와 무관하게 이름과 수치를 포함한 상세 형식입니다.
		RichHeader = RichHeader.Replace(TEXT("<coin key="), TEXT("<coin header=\"1\" key="));
		HeaderRichText->SetText(FText::FromString(RichHeader));
		HeaderRichText->RefreshTextLayout();
	}
	if (IsValid(DescriptionRichText))
	{
		DescriptionRichText->SetText(FText::FromString(FCoinDescriptionFormatter::ToRichText(SectionData.Description.ToString())));
		DescriptionRichText->RefreshTextLayout();
	}
}

FLinearColor UCoinDescriptionSectionWidget::GetTokenColor(FName Key) const
{
	FString Type, Code;
	if (!Key.ToString().Split(TEXT(":"), &Type, &Code)) return FLinearColor::White;
	if (Type == TEXT("BUFF")) Code += TEXT("Buff");
	UDataManagerSubsystem* DB = IsValid(GetGameInstance()) ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	FKeywordDefinitionData Definition;
	return IsValid(DB) && DB->TryGetKeywordByCode(FName(*Code), Definition) ? Definition.UIColor : FLinearColor::White;
}

TSharedPtr<SWidget> UCoinDescriptionSectionWidget::CreateInlineDisplay(FName Key, const FTextBlockStyle& Style, bool bHeader)
{
	FCoinDescriptionTokenData Data;
	if (!FCoinDescriptionFormatter::ResolveToken(Key, SectionData, Data)) return nullptr;
	const FCoinDescriptionInlineStyle* DisplayStyle = InlineStyles.Find(Key);
	const bool bShowDetailed = bHeader || bDetailed;
	const bool bShowValue = bHeader || !DisplayStyle || DisplayStyle->bShowValue;
	const FCoinDescriptionInlineParts Parts = FCoinDescriptionFormatter::BuildInlineParts(Data, bShowDetailed, bShowValue);
	FSlateFontInfo Font = Style.Font;
	if (bShowDetailed) Font.TypefaceFontName = DetailedTypeface;
	// 상세 수치/이름과 아이콘은 같은 DB 색상이며, 일반 수치의 흰색 표기는 유지합니다.
	const FSlateColor Color(bShowDetailed ? GetTokenColor(Key) : FLinearColor::White);
	TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox).Visibility(EVisibility::HitTestInvisible);
	auto AddText = [&Row, &Font, &Color](const FString& Text, float LeftSpacing = 0.0f)
	{
		if (Text.IsEmpty()) return;
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(LeftSpacing, 0.0f, 0.0f, 0.0f)
			[SNew(STextBlock).Text(FText::FromString(Text)).Font(Font).ColorAndOpacity(Color)];
	};
	const bool bHasVisibleValue = Data.bHasValue && bShowValue;
	// 연속 헤더는 영향 스탯의 상세 표시까지 같은 바깥 대괄호 안에 넣습니다.
	if (bHeader && Key == TEXT("KW:Continuous"))
	{
		FString Suffix = SectionData.HeaderSuffix.TrimStartAndEnd();
		Suffix.RemoveFromStart(TEXT(":"));
		Suffix.TrimStartAndEndInline();
		Suffix.RemoveFromEnd(TEXT("."));
		Suffix.TrimEndInline();
		if (Suffix.StartsWith(TEXT("<Stat>")) && Suffix.EndsWith(TEXT("</Stat>")))
			Suffix = Suffix.Mid(6, Suffix.Len() - 13).TrimStartAndEnd();
		if (Suffix == TEXT("[STAT:Count]") || Suffix == TEXT("[STAT:WeaponPower]"))
		{
			AddText(TEXT("["));
			Row->AddSlot().AutoWidth().VAlign(VAlign_Center)[CreateInlineIcon(Key, Style)];
			AddText(Data.Label.ToString() + TEXT(" : "), FMath::Max(0.0f, IconNameSpacing));
			const TSharedPtr<SWidget> StatDisplay = CreateInlineDisplay(FName(*Suffix.Mid(1, Suffix.Len() - 2)), Style, true);
			if (StatDisplay.IsValid()) Row->AddSlot().AutoWidth().VAlign(VAlign_Center)[StatDisplay.ToSharedRef()];
			AddText(TEXT("]"));
			return Row;
		}
	}
	const float ValueSpacing = FMath::Max(0.0f, IconValueSpacing);
	// 수치와 아이콘의 간격만 WBP 값으로 제어하며 대괄호/이름 서식은 유지합니다.
	if (bShowDetailed && bHasVisibleValue)
	{
		AddText(FString::FromInt(Data.Value));
		AddText(TEXT("["), ValueSpacing);
	}
	else AddText(Parts.BeforeIcon);
	Row->AddSlot().AutoWidth().VAlign(VAlign_Center)[CreateInlineIcon(Key, Style)];
	if (bShowDetailed) AddText(Data.Label.ToString() + TEXT("]"), FMath::Max(0.0f, IconNameSpacing));
	else if (bHasVisibleValue) AddText(FString::FromInt(Data.Value), ValueSpacing);
	else AddText(Parts.AfterIcon);
	return Row;
}

TSharedRef<SWidget> UCoinDescriptionSectionWidget::CreateInlineIcon(FName Key, const FTextBlockStyle& Style)
{
	UTexture2D* Texture = nullptr;
	UDataManagerSubsystem* DB = GetGameInstance() ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	// 아이콘 원본과 색상은 DB에서 가져오고 BP는 크기/표시 옵션만 지정합니다.
	if (IsValid(DB)) DB->TryGetUIIcon(Key, Texture);
	if (IsValid(Texture))
	{
		// Brush는 Slate 이미지가, Texture는 DataManager 캐시가 보유합니다.
		TSharedRef<FSlateBrush> Brush = MakeShared<FSlateBrush>();
		Brush->SetResourceObject(Texture);
		Brush->DrawAs = ESlateBrushDrawType::Image;
		const FVector2D IconSize(FMath::Max(1.0, InlineIconSize.X), FMath::Max(1.0, InlineIconSize.Y));
		Brush->ImageSize = IconSize;
		return SNew(SBox).WidthOverride(IconSize.X).HeightOverride(IconSize.Y)
			[SNew(SImage).Image_Lambda([Brush]() -> const FSlateBrush* { return &Brush.Get(); }).ColorAndOpacity(GetTokenColor(Key))];
	}
	return SNew(STextBlock).Text(FText::FromString(TEXT("?"))).Font(Style.Font).ColorAndOpacity(FLinearColor::White);
}

TSharedPtr<SWidget> UCoinDescriptionSectionWidget::CreateExpressionDisplay(const FString& Expression, const FTextBlockStyle& Style)
{
	const FCoinExpressionResult Result = FCoinDescriptionExpression::Evaluate(Expression, SectionData);
	// 잘못된 DB 수식은 원문을 남깁니다. 실패를 0으로 표시하지 않습니다.
	if (!Result.bValid)
		return SNew(STextBlock).Text(FText::FromString(Expression)).Font(Style.Font).ColorAndOpacity(FLinearColor::White);
	TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox).Visibility(EVisibility::HitTestInvisible);
	FSlateFontInfo Font = Style.Font;
	if (bDetailed) Font.TypefaceFontName = DetailedTypeface;
	auto AddText = [&Row, &Font](const FString& Text, float LeftSpacing = 0.0f)
	{
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(LeftSpacing, 0.0f, 0.0f, 0.0f)
			[SNew(STextBlock).Text(FText::FromString(Text)).Font(Font).ColorAndOpacity(FLinearColor::White)];
	};
	if (!bDetailed && !Result.bHasUnknownValue)
	{
		// 결과 모드에서만 중복 아이콘을 제거하며, 수식 자체는 변경하지 않습니다.
		for (FName Key : Result.UniqueKeys)
			Row->AddSlot().AutoWidth().VAlign(VAlign_Center)[CreateInlineIcon(Key, Style)];
		FNumberFormattingOptions Options;
		Options.SetMaximumFractionalDigits(0).SetMinimumFractionalDigits(0);
		AddText(FText::AsNumber(Result.DisplayValue, &Options).ToString(), FMath::Max(0.0f, IconValueSpacing));
	}
	else
	{
		// 슬롯에는 버프 수치가 없으므로 Shift를 누르지 않아도 기호식으로 남깁니다.
		AddText(TEXT("( "));
		for (const FCoinExpressionPart& Part : Result.Parts)
		{
			if (Part.Key.IsNone()) AddText(Part.Text);
			else if (TSharedPtr<SWidget> Display = CreateInlineDisplay(Part.Key, Style))
				Row->AddSlot().AutoWidth().VAlign(VAlign_Center)[Display.ToSharedRef()];
			AddText(TEXT(" "));
		}
		AddText(TEXT(")"));
	}
	return Row;
}
