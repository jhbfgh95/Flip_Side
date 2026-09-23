#include "UI/KeywordDescriptionWidget.h"

#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/RichTextBlock.h"

#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/CoinDescriptionFormatter.h"
#include "Styling/CoreStyle.h"

namespace KeywordDescriptionPrivate
{
	FString Escape(const FString& Text)
	{
		return Text.Replace(TEXT("&"), TEXT("&amp;")).Replace(TEXT("<"), TEXT("&lt;"))
			.Replace(TEXT(">"), TEXT("&gt;"));
	}
}

void UKeywordDescriptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (IsValid(KeywordDropdown))
		KeywordDropdown->OnSelectionChanged.AddUniqueDynamic(this, &UKeywordDescriptionWidget::HandleSelectionChanged);
	RefreshFromDatabase();
}

void UKeywordDescriptionWidget::NativeDestruct()
{
	if (IsValid(KeywordDropdown))
		KeywordDropdown->OnSelectionChanged.RemoveDynamic(this, &UKeywordDescriptionWidget::HandleSelectionChanged);
	Super::NativeDestruct();
}

void UKeywordDescriptionWidget::SetKeywordGroup(EKeywordDescriptionGroup InGroup)
{
	KeywordGroup = InGroup;
	RefreshFromDatabase();
}

void UKeywordDescriptionWidget::RefreshFromDatabase()
{
	const FName PreviousCode = SelectedKeyword.KeywordCode;
	Options.Reset();
	Definitions.Reset();
	SelectedKeyword = FKeywordDefinitionData();
	UGameInstance* Instance = GetGameInstance();
	UDataManagerSubsystem* Manager = IsValid(Instance) ? Instance->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	TArray<FKeywordDefinitionData> Keywords;
	if (IsValid(Manager)) Manager->GetAllEnabledKeywordDefinitions(Keywords);
	for (const FKeywordDefinitionData& Keyword : Keywords) Definitions.Add(Keyword.KeywordCode, Keyword);
	const TArray<FName> Codes = KeywordGroup == EKeywordDescriptionGroup::Main
		? TArray<FName>{TEXT("Attack"), TEXT("Hit"), TEXT("Mobility")}
		: KeywordGroup == EKeywordDescriptionGroup::Item
			? TArray<FName>{TEXT("Attack"), TEXT("Hit"), TEXT("Instant")}
			: TArray<FName>{TEXT("Continuous"), TEXT("Strike"), TEXT("Absorb")};
	// 재구성 중 발생하는 ClearOptions 이벤트는 선택 처리에서 제외합니다.
	if (IsValid(KeywordDropdown)) KeywordDropdown->OnSelectionChanged.RemoveDynamic(this, &UKeywordDescriptionWidget::HandleSelectionChanged);
	if (IsValid(KeywordDropdown)) KeywordDropdown->ClearOptions();
	int32 SelectedIndex = 0;
	for (FName Code : Codes)
	{
		const FKeywordDefinitionData* Data = Definitions.Find(Code);
		if (!Data) continue;
		if (Code == PreviousCode) SelectedIndex = Options.Num();
		Options.Add(*Data);
		if (IsValid(KeywordDropdown)) KeywordDropdown->AddOption(Data->DisplayName.ToString());
	}
	if (Options.IsValidIndex(SelectedIndex)) SelectedKeyword = Options[SelectedIndex];
	if (IsValid(KeywordDropdown))
	{
		KeywordDropdown->SetIsEnabled(!Options.IsEmpty());
		if (!Options.IsEmpty()) KeywordDropdown->SetSelectedIndex(SelectedIndex);
		KeywordDropdown->OnSelectionChanged.AddUniqueDynamic(this, &UKeywordDescriptionWidget::HandleSelectionChanged);
	}
	RefreshDescription();
}

void UKeywordDescriptionWidget::HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!IsValid(KeywordDropdown)) return;
	const int32 Index = KeywordDropdown->GetSelectedIndex();
	if (!Options.IsValidIndex(Index)) return;
	SelectedKeyword = Options[Index];
	RefreshDescription();
}

void UKeywordDescriptionWidget::RefreshDescription()
{
	// 선택한 키워드의 DB 아이콘/색상을 직접 적용합니다. BP Brush 크기는 보존합니다.
	if (IsValid(KeywordIcon))
	{
		const bool bHasIcon = IsValid(SelectedKeyword.Icon);
		if (bHasIcon)
		{
			if (UMaterialInstanceDynamic* Material = KeywordIcon->GetDynamicMaterial())
				Material->SetTextureParameterValue(TEXT("Icon"), SelectedKeyword.Icon);
			else
			{
				FSlateBrush Brush = KeywordIcon->GetBrush();
				Brush.SetResourceObject(SelectedKeyword.Icon);
				KeywordIcon->SetBrush(Brush);
			}
			KeywordIcon->SetColorAndOpacity(SelectedKeyword.UIColor);
		}
		KeywordIcon->SetVisibility(bHasIcon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	// SetKeywordGroup은 컨테이너에 추가되기 전에도 호출됩니다. Slate 스타일 생성 이후에 갱신합니다.
	if (!IsValid(KeywordDescriptionText) || !KeywordDescriptionText->GetCachedWidget().IsValid()) return;
	TextStyles = NewObject<UDataTable>(this);
	TextStyles->RowStruct = FRichTextStyleRow::StaticStruct();
	FRichTextStyleRow DefaultRow;
	DefaultRow.TextStyle = KeywordDescriptionText->GetCurrentDefaultTextStyle();
	// 디자이너의 Override Default Style을 보존하고, 미설정 폰트는 기본 UI 폰트로 보완합니다.
	if (!IsValid(DefaultRow.TextStyle.Font.FontObject) && !DefaultRow.TextStyle.Font.CompositeFont.IsValid())
		DefaultRow.TextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 16));
	TextStyles->AddRow(TEXT("Default"), DefaultRow);
	FCoinDescriptionSectionData Context;
	for (const auto& Pair : Definitions) Context.KeywordLabels.Add(Pair.Key, Pair.Value.DisplayName);
	const FString Raw = SelectedKeyword.Description.ToString().Replace(TEXT("\\n"), TEXT("\n")).Replace(TEXT("\r"), TEXT(""));
	FString Result;
	int32 TextCursor = 0;
	while (TextCursor < Raw.Len())
	{
		const int32 Open = Raw.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, TextCursor);
		if (Open == INDEX_NONE) { Result += KeywordDescriptionPrivate::Escape(Raw.Mid(TextCursor)); break; }
		Result += KeywordDescriptionPrivate::Escape(Raw.Mid(TextCursor, Open - TextCursor));
		const int32 Close = Raw.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Open + 1);
		if (Close == INDEX_NONE) { Result += KeywordDescriptionPrivate::Escape(Raw.Mid(Open)); break; }
		const FString Key = Raw.Mid(Open + 1, Close - Open - 1);
		FCoinDescriptionTokenData Token;
		if (FCoinDescriptionFormatter::ResolveToken(FName(*Key), Context, Token))
		{
			FRichTextStyleRow Row = DefaultRow;
			FString Type, Code;
			Key.Split(TEXT(":"), &Type, &Code);
			if (Type == TEXT("KW") || Type == TEXT("BUFF"))
				if (const FKeywordDefinitionData* Data = Definitions.Find(FName(*Code))) Row.TextStyle.SetColorAndOpacity(Data->UIColor);
			if (const FLinearColor* Color = TokenColorOverrides.Find(FName(*Key))) Row.TextStyle.SetColorAndOpacity(*Color);
			const FString StyleName = Key.Replace(TEXT(":"), TEXT("_"));
			TextStyles->AddRow(FName(*StyleName), Row);
			// TODO(KeywordInlineIcon): KW/STAT/BUFF 아이콘 확정 후 이 위치에 전용 decorator를 연결합니다.
			// 아이콘 미연결 상태에서는 색상이 적용된 이름을 표시하고 임의의 수치는 표시하지 않습니다.
			Result += TEXT("<") + StyleName + TEXT(">") + KeywordDescriptionPrivate::Escape(Token.Label.ToString()) + TEXT("</>");
		}
		else Result += KeywordDescriptionPrivate::Escape(Raw.Mid(Open, Close - Open + 1));
		TextCursor = Close + 1;
	}
	KeywordDescriptionText->SetTextStyleSet(TextStyles);
	KeywordDescriptionText->SetText(FText::FromString(Result));
}
