#include "UI/CoinDescriptionRichTextDecorator.h"
#include "UI/CoinDescriptionSectionWidget.h"
#include "UI/CardGoldTierTableWidget.h"
#include "UI/CardGoldAppliedInfoWidget.h"
#include "UI/BuffsInfoPopup.h"
#include "Components/RichTextBlock.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/SlateWidgetRun.h"
#include "Misc/Base64.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace CoinDescriptionRichTextPrivate
{
	// FRichTextDecorator의 기본 기준선은 삽입 위젯과 글자의 높이가 같다고 가정합니다.
	// 버프 설명은 더 큰 아이콘을 포함하므로 내부 글자가 본문과 같은 기준선에 놓이도록 보정합니다.
	class FBuffStatDecorator : public ITextDecorator
	{
	public:
		FBuffStatDecorator(URichTextBlock* InOwner, UBuffsInfoPopup* InPopup) : Owner(InOwner), Popup(InPopup) {}
		virtual bool Supports(const FTextRunParseResults& Run, const FString& Text) const override
		{
			return Run.Name == TEXT("coin") && Run.MetaData.Contains(TEXT("key"));
		}
		virtual TSharedRef<ISlateRun> Create(const TSharedRef<FTextLayout>& TextLayout,
			const FTextRunParseResults& ParsedRun, const FString& OriginalText,
			const TSharedRef<FString>& ModelText, const ISlateStyle* Style) override
		{
			const int32 BeginIndex = ModelText->Len();
			FTextRunInfo RunInfo(ParsedRun.Name, FText::FromString(OriginalText.Mid(
				ParsedRun.ContentRange.BeginIndex, ParsedRun.ContentRange.EndIndex - ParsedRun.ContentRange.BeginIndex)));
			for (const TPair<FString, FTextRange>& Pair : ParsedRun.MetaData)
				RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
			const FTextBlockStyle TextStyle = Owner.IsValid() ? Owner->GetCurrentDefaultTextStyle() : FTextBlockStyle();
			const TSharedPtr<SWidget> Display = Popup.IsValid() ? Popup->CreateStatDisplay(RunInfo, TextStyle) : nullptr;
			if (!Display.IsValid())
			{
				*ModelText += RunInfo.Content.ToString();
				return FSlateTextRun::Create(RunInfo, ModelText, TextStyle, FTextRange(BeginIndex, ModelText->Len()));
			}
			*ModelText += TEXT('\u200B');
			const FSlateFontInfo Font = TextStyle.Font;
			const FVector2D ShadowOffset = TextStyle.ShadowOffset;
			const TAttribute<int16> Baseline = TAttribute<int16>::CreateLambda([Display, Font, ShadowOffset]()
			{
				const TSharedRef<FSlateFontMeasure> Measure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
				const float TextHeight = Measure->GetMaxCharacterHeight(Font, 1.f) +
					FMath::Abs(ShadowOffset.Y) + Font.OutlineSettings.OutlineSize;
				const float CenterOffset = FMath::Max(0.f, static_cast<float>(Display->GetDesiredSize().Y) - TextHeight) * 0.5f;
				return static_cast<int16>(FMath::RoundToInt(Measure->GetBaseline(Font) -
					FMath::Min(0.0, ShadowOffset.Y) - Font.OutlineSettings.OutlineSize - CenterOffset));
			});
			return FSlateWidgetRun::Create(TextLayout, RunInfo, ModelText,
				FSlateWidgetRun::FWidgetRunInfo(Display.ToSharedRef(), Baseline), FTextRange(BeginIndex, ModelText->Len()));
		}
	private:
		TWeakObjectPtr<URichTextBlock> Owner;
		TWeakObjectPtr<UBuffsInfoPopup> Popup;
	};

	class FInlineDecorator : public FRichTextDecorator
	{
	public:
		explicit FInlineDecorator(URichTextBlock* InOwner) : FRichTextDecorator(InOwner),
			Section(InOwner ? InOwner->GetTypedOuter<UCoinDescriptionSectionWidget>() : nullptr),
			GoldTierTable(InOwner ? InOwner->GetTypedOuter<UCardGoldTierTableWidget>() : nullptr),
			GoldAppliedInfo(InOwner ? InOwner->GetTypedOuter<UCardGoldAppliedInfoWidget>() : nullptr) {}
		virtual bool Supports(const FTextRunParseResults& Run, const FString& Text) const override
		{
			return (Run.Name == TEXT("coin") && Run.MetaData.Contains(TEXT("key"))) ||
				(Run.Name == TEXT("coinexpr") && Run.MetaData.Contains(TEXT("expr")));
		}
	protected:
		virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& Run, const FTextBlockStyle& Style) const override
		{
			if (GoldAppliedInfo.IsValid()) return GoldAppliedInfo->CreateStatDisplay(Run, Style);
			if (GoldTierTable.IsValid()) return GoldTierTable->CreateStatDisplay(Run, Style);
			if (!Section.IsValid()) return nullptr;
			if (const FString* Encoded = Run.MetaData.Find(TEXT("expr")))
			{
				FString Expression;
				return FBase64::Decode(*Encoded, Expression) ? Section->CreateExpressionDisplay(Expression, Style) : nullptr;
			}
			const FString* Key = Run.MetaData.Find(TEXT("key"));
			return Key ? Section->CreateInlineDisplay(FName(**Key), Style, Run.MetaData.Contains(TEXT("header"))) : nullptr;
		}
	private:
		TWeakObjectPtr<UCoinDescriptionSectionWidget> Section;
		TWeakObjectPtr<UCardGoldTierTableWidget> GoldTierTable;
		TWeakObjectPtr<UCardGoldAppliedInfoWidget> GoldAppliedInfo;
	};
}
TSharedPtr<ITextDecorator> UCoinDescriptionRichTextDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	if (UBuffsInfoPopup* Popup = InOwner ? InOwner->GetTypedOuter<UBuffsInfoPopup>() : nullptr)
		return MakeShared<CoinDescriptionRichTextPrivate::FBuffStatDecorator>(InOwner, Popup);
	return MakeShared<CoinDescriptionRichTextPrivate::FInlineDecorator>(InOwner);
}

TSharedPtr<SWidget> UCoinDescriptionRichTextDecorator::CreateDetailedStatDisplay(const UUserWidget* Widget,
	const FTextRunInfo& Run, const FTextBlockStyle& Style, const FVector2D& IconSize,
	float ValueSpacing, float NameSpacing, FName Typeface)
{
	const FString* KeyText = Run.MetaData.Find(TEXT("key"));
	if (!IsValid(Widget) || !KeyText) return nullptr;
	const FName Keys[] = { TEXT("STAT:AttackPower"), TEXT("STAT:WeaponPower"), TEXT("STAT:Count"),
		TEXT("STAT:AttackRange"), TEXT("STAT:AbilityRange") };
	const TCHAR* const Names[] = { TEXT("공격력"), TEXT("무기력"), TEXT("횟수"), TEXT("공격사거리"), TEXT("능력사거리") };
	const FName Key(**KeyText);
	int32 StatIndex = INDEX_NONE;
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Keys); ++Index)
	{
		if (Key == Keys[Index])
		{
			StatIndex = Index;
			break;
		}
	}
	if (StatIndex == INDEX_NONE) return nullptr;
	UGameInstance* Instance = Widget->GetGameInstance();
	UDataManagerSubsystem* Manager = IsValid(Instance) ? Instance->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	UTexture2D* Texture = nullptr;
	FKeywordDefinitionData Definition;
	if (IsValid(Manager)) Manager->TryGetUIIcon(Key, Texture);
	const bool bHasDefinition = IsValid(Manager) && Manager->TryGetKeywordByCode(FName(*KeyText->RightChop(5)), Definition);
	const FSlateColor Color = bHasDefinition ? FSlateColor(Definition.UIColor) : Style.ColorAndOpacity;
	const FString Label = bHasDefinition && !Definition.DisplayName.IsEmpty() ? Definition.DisplayName.ToString() : Names[StatIndex];
	FSlateFontInfo Font = Style.Font;
	Font.TypefaceFontName = Typeface;
	TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox).Visibility(EVisibility::HitTestInvisible);
	auto AddText = [&Row, &Font, &Color](const FString& Text, float LeftSpacing = 0.f)
	{
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(LeftSpacing, 0.f, 0.f, 0.f)
			[SNew(STextBlock).Text(FText::FromString(Text)).Font(Font).ColorAndOpacity(Color)];
	};
	const FString* Value = Run.MetaData.Find(TEXT("value"));
	if (Value) AddText(*Value);
	AddText(TEXT("["), Value ? FMath::Max(0.f, ValueSpacing) : 0.f);
	if (IsValid(Texture))
	{
		// 원본 Texture는 DataManager가, Brush는 Slate 람다가 보유합니다.
		TSharedRef<FSlateBrush> Brush = MakeShared<FSlateBrush>();
		Brush->SetResourceObject(Texture);
		Brush->DrawAs = ESlateBrushDrawType::Image;
		const FVector2D Size(FMath::Max(1.0, IconSize.X), FMath::Max(1.0, IconSize.Y));
		Brush->ImageSize = Size;
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
			[SNew(SBox).WidthOverride(Size.X).HeightOverride(Size.Y)
				[SNew(SImage).Image_Lambda([Brush]() -> const FSlateBrush* { return &Brush.Get(); }).ColorAndOpacity(Color)]];
	}
	AddText(Label + TEXT("]"), IsValid(Texture) ? FMath::Max(0.f, NameSpacing) : 0.f);
	return Row;
}
