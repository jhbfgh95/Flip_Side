#include "UI/CoinDescriptionRichTextDecorator.h"
#include "UI/CoinDescriptionSectionWidget.h"
#include "UI/CardGoldTierTableWidget.h"
#include "UI/CardGoldAppliedInfoWidget.h"
#include "Components/RichTextBlock.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Misc/Base64.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace CoinDescriptionRichTextPrivate
{
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
