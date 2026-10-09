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
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"
#include "Layout/Children.h"
#include "Rendering/SlateRenderer.h"

namespace KeywordDescriptionPrivate
{
	FString Escape(const FString& Text)
	{
		return Text.Replace(TEXT("&"), TEXT("&amp;")).Replace(TEXT("<"), TEXT("&lt;"))
			.Replace(TEXT(">"), TEXT("&gt;"));
	}

	// 화면에 그릴 설명 조각입니다. 스타일이 있는 조각(키워드 이름)은 줄바꿈 위치에서 제외합니다.
	struct FTextPiece
	{
		FString Text;
		FString StyleName;
	};

	// 값이 작을수록 우선합니다: 문장 끝 > 쉼표 > 띄어쓰기.
	enum class EBreakKind : uint8 { Sentence = 0, Clause = 1, Word = 2 };

	struct FBreakCandidate
	{
		int32 PieceIndex;
		int32 CharIndex;
		int32 PlainIndex;
	};

	EBreakKind ClassifyBreak(const FString& Plain, int32 SpaceIndex)
	{
		for (int32 Index = SpaceIndex - 1; Index >= 0; --Index)
		{
			const TCHAR Char = Plain[Index];
			if (FChar::IsWhitespace(Char)) continue;
			if (Char == TEXT('.') || Char == TEXT('!') || Char == TEXT('?') || Char == TEXT('…')) return EBreakKind::Sentence;
			if (Char == TEXT(',')) return EBreakKind::Clause;
			break;
		}
		return EBreakKind::Word;
	}

	// 한 줄에 들어가면 그대로 두고, 넘치면 두 줄 안에서 문장 끝 > 쉼표 > 띄어쓰기 순으로 나눌 공백을 고릅니다.
	// 같은 종류끼리는 두 줄 길이가 비슷한 쪽을 고릅니다. 두 줄로도 들어가지 않으면 false를 반환합니다.
	bool InsertLineBreak(TArray<FTextPiece>& Pieces, const FSlateFontInfo& Font, float AvailableWidth)
	{
		FString Plain;
		TArray<FBreakCandidate> Candidates;
		for (int32 PieceIndex = 0; PieceIndex < Pieces.Num(); ++PieceIndex)
		{
			const FTextPiece& Piece = Pieces[PieceIndex];
			if (Piece.StyleName.IsEmpty())
				for (int32 CharIndex = 0; CharIndex < Piece.Text.Len(); ++CharIndex)
					if (Piece.Text[CharIndex] == TEXT(' ')) Candidates.Add({PieceIndex, CharIndex, Plain.Len() + CharIndex});
			Plain += Piece.Text;
		}
		// DB 설명에 직접 넣은 줄바꿈은 작성자의 의도로 보고 그대로 둡니다.
		if (Plain.Contains(TEXT("\n"))) return true;
		const TSharedRef<FSlateFontMeasure> Measure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		auto WidthOf = [&Measure, &Plain, &Font](int32 Start, int32 End)
		{
			return static_cast<float>(Measure->Measure(FStringView(Plain).Mid(Start, End - Start), Font).X);
		};
		if (WidthOf(0, Plain.Len()) <= AvailableWidth) return true;
		const FBreakCandidate* Best = nullptr;
		float BestScore = TNumericLimits<float>::Max();
		for (const FBreakCandidate& Candidate : Candidates)
		{
			const float First = WidthOf(0, Candidate.PlainIndex);
			const float Second = WidthOf(Candidate.PlainIndex + 1, Plain.Len());
			if (First > AvailableWidth || Second > AvailableWidth) continue;
			const float Score = static_cast<float>(ClassifyBreak(Plain, Candidate.PlainIndex)) * 10.f
				+ FMath::Abs(First - Second) / AvailableWidth;
			if (Score < BestScore)
			{
				BestScore = Score;
				Best = &Candidate;
			}
		}
		if (!Best) return false;
		Pieces[Best->PieceIndex].Text[Best->CharIndex] = TEXT('\n');
		return true;
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
	DropdownArrowImages.Reset();
	Super::NativeDestruct();
}

void UKeywordDescriptionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateDropdownArrow(InDeltaTime);
	// 설명 칸 폭이 처음 정해지거나 바뀌면 줄바꿈 위치를 다시 계산합니다.
	if (IsValid(KeywordDescriptionText))
	{
		const float Width = KeywordDescriptionText->GetCachedGeometry().GetLocalSize().X;
		if (Width > 1.f && !FMath::IsNearlyEqual(Width, LastDescriptionWidth, 0.5f)) RefreshDescription();
	}
}

void UKeywordDescriptionWidget::CollectDropdownArrowImages()
{
	DropdownArrowImages.Reset();
	if (!IsValid(KeywordDropdown) || !KeywordDropdown->GetCachedWidget().IsValid()) return;
	// SComboButton의 SButton 안에는 선택 텍스트(STextBlock)와 화살표 SImage(그림자 포함)만 있습니다.
	TFunction<void(const TSharedRef<SWidget>&, bool)> Visit;
	Visit = [this, &Visit](const TSharedRef<SWidget>& Widget, bool bInsideButton)
	{
		const FName Type = Widget->GetType();
		if (bInsideButton && Type == TEXT("SImage"))
		{
			DropdownArrowImages.Add(Widget);
			return;
		}
		FChildren* Children = Widget->GetChildren();
		for (int32 Index = 0; Index < Children->Num(); ++Index)
			Visit(Children->GetChildAt(Index), bInsideButton || Type == TEXT("SButton"));
	};
	Visit(KeywordDropdown->GetCachedWidget().ToSharedRef(), false);
}

void UKeywordDescriptionWidget::UpdateDropdownArrow(float DeltaTime)
{
	if (!IsValid(KeywordDropdown)) return;
	const float TargetAngle = KeywordDropdown->IsOpen() ? 180.f : 0.f;
	const bool bHasImages = !DropdownArrowImages.IsEmpty() && DropdownArrowImages[0].IsValid();
	if (bHasImages && DropdownArrowAngle == TargetAngle) return;
	// 콤보 위젯이 재생성되면 화살표도 새로 만들어지므로 다시 찾고 현재 각도를 적용합니다.
	if (!bHasImages)
	{
		CollectDropdownArrowImages();
		if (DropdownArrowImages.IsEmpty()) return;
	}
	DropdownArrowAngle = ArrowRotateDuration > 0.f
		? FMath::FInterpConstantTo(DropdownArrowAngle, TargetAngle, DeltaTime, 180.f / ArrowRotateDuration)
		: TargetAngle;
	if (FMath::IsNearlyEqual(DropdownArrowAngle, TargetAngle, 0.01f)) DropdownArrowAngle = TargetAngle;
	const TOptional<FSlateRenderTransform> Rotation(FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(DropdownArrowAngle))));
	for (const TWeakPtr<SWidget>& WeakImage : DropdownArrowImages)
		if (const TSharedPtr<SWidget> Image = WeakImage.Pin())
		{
			Image->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
			Image->SetRenderTransform(Rotation);
		}
}

void UKeywordDescriptionWidget::SetKeywordGroup(EKeywordDescriptionGroup InGroup)
{
	KeywordGroup = InGroup;
	bUseContextKeywords = false;
	ContextKeywordCodes.Reset();
	RefreshFromDatabase();
}

void UKeywordDescriptionWidget::SetContextKeywords(const TArray<FName>& KeywordCodes)
{
	bUseContextKeywords = true;
	ContextKeywordCodes = KeywordCodes;
	// 새 책갈피에서는 기존 드롭다운 선택 대신 정해진 순서의 첫 키워드부터 표시합니다.
	SelectedKeyword = FKeywordDefinitionData();
	RefreshFromDatabase();
	SetVisibility(Options.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
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
		if (bUseContextKeywords && !ContextKeywordCodes.Contains(Code)) continue;
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
	TArray<KeywordDescriptionPrivate::FTextPiece> Pieces;
	int32 TextCursor = 0;
	while (TextCursor < Raw.Len())
	{
		const int32 Open = Raw.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, TextCursor);
		if (Open == INDEX_NONE) { Pieces.Add({Raw.Mid(TextCursor), FString()}); break; }
		Pieces.Add({Raw.Mid(TextCursor, Open - TextCursor), FString()});
		const int32 Close = Raw.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Open + 1);
		if (Close == INDEX_NONE) { Pieces.Add({Raw.Mid(Open), FString()}); break; }
		const FString Key = Raw.Mid(Open + 1, Close - Open - 1);
		FCoinDescriptionTokenData Token;
		if (FCoinDescriptionFormatter::ResolveToken(FName(*Key), Context, Token))
		{
			FRichTextStyleRow Row = DefaultRow;
			FString Type, Code;
			Key.Split(TEXT(":"), &Type, &Code);
			if (Type == TEXT("BUFF")) Code += TEXT("Buff");
			const FKeywordDefinitionData* Definition = Definitions.Find(FName(*Code));
			Row.TextStyle.SetColorAndOpacity(Definition ? Definition->UIColor : FLinearColor::White);
			const FString StyleName = Key.Replace(TEXT(":"), TEXT("_"));
			TextStyles->AddRow(FName(*StyleName), Row);
			// TODO(KeywordInlineIcon): KW/STAT/BUFF 아이콘 확정 후 이 위치에 전용 decorator를 연결합니다.
			// 아이콘 미연결 상태에서는 색상이 적용된 이름을 표시하고 임의의 수치는 표시하지 않습니다.
			Pieces.Add({Token.Label.ToString(), StyleName});
		}
		else Pieces.Add({Raw.Mid(Open, Close - Open + 1), FString()});
		TextCursor = Close + 1;
	}
	// 실제 칸 폭은 첫 레이아웃 이후에 정해집니다. 그 전에는 자동 줄바꿈에 맡기고 NativeTick에서 다시 계산합니다.
	const float AvailableWidth = KeywordDescriptionText->GetCachedGeometry().GetLocalSize().X;
	LastDescriptionWidth = AvailableWidth;
	if (AvailableWidth > 1.f && FSlateApplication::IsInitialized()
		&& !KeywordDescriptionPrivate::InsertLineBreak(Pieces, DefaultRow.TextStyle.Font, AvailableWidth - 4.f))
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[KeywordDescription] %s 설명이 두 줄을 넘습니다. 설명 길이를 확인하세요."),
			*SelectedKeyword.KeywordCode.ToString());
#endif
	}
	FString Result;
	for (const KeywordDescriptionPrivate::FTextPiece& Piece : Pieces)
		Result += Piece.StyleName.IsEmpty() ? KeywordDescriptionPrivate::Escape(Piece.Text)
			: TEXT("<") + Piece.StyleName + TEXT(">") + KeywordDescriptionPrivate::Escape(Piece.Text) + TEXT("</>");
	KeywordDescriptionText->SetTextStyleSet(TextStyles);
	KeywordDescriptionText->SetText(FText::FromString(Result));
}
