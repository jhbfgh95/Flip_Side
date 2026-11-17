#include "UI/CardGoldTierTableWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/RichTextBlock.h"
#include "DataTypes/CardTypes.h"
#include "DataTypes/KeywordDataTypes.h"
#include "Engine/GameInstance.h"
#include "Styling/CoreStyle.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/CoinDescriptionRichTextDecorator.h"

namespace CardGoldTierTablePrivate
{
	const FName StatKeys[] = { TEXT("STAT:AttackPower"), TEXT("STAT:WeaponPower"), TEXT("STAT:Count"),
		TEXT("STAT:AttackRange"), TEXT("STAT:AbilityRange") };
}

UCardGoldTierTableWidget::UCardGoldTierTableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ValueTextStyle = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>(TEXT("NormalText"));
	ValueTextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 15));
	ValueTextStyle.SetColorAndOpacity(FSlateColor(FLinearColor::White));
	HeaderTextStyle = ValueTextStyle;
	HeaderTextStyle.Font.TypefaceFontName = TEXT("Bold");
}

void UCardGoldTierTableWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsValid(WidgetTree) || IsValid(GoldTierGrid)) return;
	const UPanelWidget* EmptyRoot = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!IsValid(WidgetTree->RootWidget) || (IsValid(EmptyRoot) && EmptyRoot->GetChildrenCount() == 0))
	{
		GoldTierGrid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), TEXT("GoldTierGrid"));
		WidgetTree->RootWidget = GoldTierGrid;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[CardGoldTierTable] WBP에 GoldTierGrid 이름의 GridPanel을 배치하세요."));
	}
}

void UCardGoldTierTableWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshTable();
}

void UCardGoldTierTableWidget::RefreshTable()
{
	if (!IsValid(WidgetTree) || !IsValid(GoldTierGrid)) return;
	AddCell(0, 0, NSLOCTEXT("CardGoldTierTable", "RemainingGold", "남은\n골드"));
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(CardGoldTierTablePrivate::StatKeys); ++Index)
	{
		const FString Tag = FString::Printf(TEXT("<coin key=\"%s\"/>"), *CardGoldTierTablePrivate::StatKeys[Index].ToString());
		AddCell(Index + 1, 0, FText::FromString(Tag));
	}

	UGameInstance* Instance = GetGameInstance();
	UDataManagerSubsystem* Manager = IsValid(Instance) ? Instance->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	FCardData Card;
	if (!IsValid(Manager) || !Manager->TryGetCard(4, Card))
	{
		UE_LOG(LogTemp, Warning, TEXT("[CardGoldTierTable] 더 많은 황금(ID 4)의 데이터를 가져오지 못했습니다."));
		return;
	}
	Card.GoldTiers.Sort([](const FCardGoldTierData& A, const FCardGoldTierData& B)
	{
		return A.MinimumGold < B.MinimumGold;
	});
	for (int32 Index = 0; Index < Card.GoldTiers.Num(); ++Index)
	{
		const FCardGoldTierData& Tier = Card.GoldTiers[Index];
		AddCell(0, Index + 1, FText::Format(NSLOCTEXT("CardGoldTierTable", "MinimumGold", "{0}\n골드\n이상"),
			FText::AsNumber(Tier.MinimumGold)));
		// 각 구간의 최종 증가량을 그대로 표시합니다. 이전 구간과 누적하지 않습니다.
		const FCoinCardModifiers& Modifiers = Tier.Modifiers;
		const int32 Values[] = { Modifiers.AttackAdd, Modifiers.BehaviorAdd, Modifiers.CountAdd,
			Modifiers.RangeAdd, Modifiers.AbilityRangeAdd };
		for (int32 Row = 0; Row < UE_ARRAY_COUNT(Values); ++Row)
		{
			AddCell(Row + 1, Index + 1, FText::FromString(FString::Printf(TEXT("%+d"), Values[Row])));
		}
	}
}

void UCardGoldTierTableWidget::AddCell(int32 Row, int32 Column, const FText& Text)
{
	if (!IsValid(WidgetTree) || !IsValid(GoldTierGrid)) return;
	UBorder* Cell = nullptr;
	for (UWidget* Child : GoldTierGrid->GetAllChildren())
	{
		if (!IsValid(Child)) continue;
		const UGridSlot* ExistingSlot = Cast<UGridSlot>(Child->Slot);
		if (!IsValid(ExistingSlot) || ExistingSlot->GetRow() != Row || ExistingSlot->GetColumn() != Column) continue;
		Cell = Cast<UBorder>(Child);
		if (!IsValid(Cell)) return;
		break;
	}
	// 에디터에서 배치한 셀은 그대로 사용해 배경색과 배치를 보존합니다.
	const bool bNewCell = !IsValid(Cell);
	if (bNewCell) Cell = WidgetTree->ConstructWidget<UBorder>();
	URichTextBlock* RichText = bNewCell ? WidgetTree->ConstructWidget<URichTextBlock>()
		: Cast<URichTextBlock>(Cell->GetContent());
	if (!IsValid(Cell) || !IsValid(RichText)) return;
	const bool bHeader = Row == 0 || Column == 0;
	if (bNewCell)
	{
		Cell->SetPadding(CellPadding);
		Cell->SetHorizontalAlignment(HAlign_Center);
		Cell->SetVerticalAlignment(VAlign_Center);
		Cell->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	FTextBlockStyle TextStyle = bHeader ? HeaderTextStyle : ValueTextStyle;
	if (Row > 0 && Column > 0 && Row <= UE_ARRAY_COUNT(CardGoldTierTablePrivate::StatKeys))
	{
		// 내용 셀에는 아이콘과 이름 없이 스탯 색상을 적용한 증가량만 표시합니다.
		TextStyle.Font.TypefaceFontName = DetailedTypeface;
		UGameInstance* Instance = GetGameInstance();
		UDataManagerSubsystem* Manager = IsValid(Instance) ? Instance->GetSubsystem<UDataManagerSubsystem>() : nullptr;
		const FName StatCode(*CardGoldTierTablePrivate::StatKeys[Row - 1].ToString().RightChop(5));
		FKeywordDefinitionData Definition;
		if (IsValid(Manager) && Manager->TryGetKeywordByCode(StatCode, Definition))
			TextStyle.SetColorAndOpacity(FSlateColor(Definition.UIColor));
	}
	RichText->SetDefaultTextStyle(TextStyle);
	RichText->SetAutoWrapText(false);
	if (Row == 0)
		RichText->SetJustification(ETextJustify::Center);
	if (Row > 0 && Column == 0)
		RichText->SetDecorators({ UCoinDescriptionRichTextDecorator::StaticClass() });
	RichText->SetText(Text);
	if (bNewCell)
	{
		Cell->SetContent(RichText);
		if (UGridSlot* CellSlot = GoldTierGrid->AddChildToGrid(Cell, Row, Column); IsValid(CellSlot))
		{
			CellSlot->SetPadding(FMargin(0.5f));
			CellSlot->SetHorizontalAlignment(HAlign_Fill);
			CellSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}

TSharedPtr<SWidget> UCardGoldTierTableWidget::CreateStatDisplay(const FTextRunInfo& Run, const FTextBlockStyle& Style)
{
	return UCoinDescriptionRichTextDecorator::CreateDetailedStatDisplay(this, Run, Style,
		InlineIconSize, IconValueSpacing, IconNameSpacing, DetailedTypeface);
}
