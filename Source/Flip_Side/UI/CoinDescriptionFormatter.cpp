#include "UI/CoinDescriptionFormatter.h"

namespace CoinDescriptionFormatterPrivate
{
	FString Normalize(const FString& Text)
	{
		return Text.Replace(TEXT("\\n"), TEXT("\n")).Replace(TEXT("\r"), TEXT(""));
	}

	FString Escape(const FString& Text)
	{
		return Text.Replace(TEXT("&"), TEXT("&amp;")).Replace(TEXT("<"), TEXT("&lt;"))
			.Replace(TEXT(">"), TEXT("&gt;")).Replace(TEXT("\""), TEXT("&quot;"));
	}

	bool ReadKeyword(FString& Remaining, FName& OutCode)
	{
		Remaining.TrimStartInline();
		if (!Remaining.StartsWith(TEXT("[KW:"))) return false;
		const int32 End = Remaining.Find(TEXT("]"));
		if (End == INDEX_NONE) return false;
		OutCode = FName(*Remaining.Mid(4, End - 4));
		Remaining.RightChopInline(End + 1);
		return true;
	}

	bool MainKeyword(FName Code, ECoinDescriptionKeyword& OutKeyword)
	{
		if (Code == TEXT("Attack")) OutKeyword = ECoinDescriptionKeyword::Attack;
		else if (Code == TEXT("Mobility")) OutKeyword = ECoinDescriptionKeyword::Mobility;
		else if (Code == TEXT("Hit")) OutKeyword = ECoinDescriptionKeyword::Hit;
		else return false;
		return true;
	}

	bool AdditionalKeyword(FName Code)
	{
		return Code == TEXT("Continuous") || Code == TEXT("Absorb") || Code == TEXT("Strike");
	}

	// 본문 중간의 [KW:Mobility] 참조와 구별하여, 헤더만으로 구성된 행만 구간을 만듭니다.
	bool ReadHeader(const FString& Line, FCoinDescriptionSectionData& Section)
	{
		FString Remaining = Line.TrimStartAndEnd();
		if (!ReadKeyword(Remaining, Section.MainKeywordCode) || !MainKeyword(Section.MainKeywordCode, Section.MainKeyword)) return false;
		while (Remaining.TrimStart().StartsWith(TEXT("[KW:")))
		{
			FName Code;
			if (!ReadKeyword(Remaining, Code) || !AdditionalKeyword(Code)) return false;
			Section.AdditionalKeywordCodes.Add(Code);
		}
		Remaining.TrimStartAndEndInline();
		if (!Remaining.IsEmpty())
		{
			// 연속 뒤의 수치 표시는 원문 그대로 남깁니다. 능력 실행 조건을 만들지 않습니다.
			if (!Remaining.StartsWith(TEXT(":"))) return false;
			FString CountText = Remaining.RightChop(1).TrimStartAndEnd();
			CountText.RemoveFromEnd(TEXT("."));
			CountText.TrimEndInline();
			if (CountText != TEXT("[STAT:Count]") && CountText != TEXT("[STAT:WeaponPower]") && !CountText.IsNumeric()) return false;
			Section.HeaderSuffix = Remaining;
		}
		return true;
	}
}

FCoinWeaponDescriptionData FCoinDescriptionFormatter::Parse(const FString& RawDescription,
	const TArray<FKeywordDefinitionData>& Keywords, const FWeaponNumericStats& BaseStats)
{
	FCoinWeaponDescriptionData Result;
	TMap<FName, FText> Labels;
	for (const FKeywordDefinitionData& Keyword : Keywords)
	{
		if (Keyword.bEnabled) Labels.Add(Keyword.KeywordCode, Keyword.DisplayName);
	}
	TArray<FString> Lines;
	CoinDescriptionFormatterPrivate::Normalize(RawDescription).ParseIntoArrayLines(Lines, false);
	FCoinDescriptionSectionData* Current = nullptr;
	for (const FString& Line : Lines)
	{
		FCoinDescriptionSectionData Header;
		if (CoinDescriptionFormatterPrivate::ReadHeader(Line, Header))
		{
			Current = &Result.Sections.Add_GetRef(Header);
			Current->BaseStats = BaseStats;
			Current->KeywordLabels = Labels;
			const FText* Label = Labels.Find(Current->MainKeywordCode);
			Current->MainKeywordLabel = Label ? *Label : FText::FromName(Current->MainKeywordCode);
			for (FName Code : Current->AdditionalKeywordCodes)
			{
				const FText* AdditionalLabel = Labels.Find(Code);
				Current->AdditionalKeywords.Add(AdditionalLabel ? *AdditionalLabel : FText::FromName(Code));
			}
		}
		else if (Current)
		{
			FString Body = Current->Description.ToString();
			if (!Body.IsEmpty()) Body += TEXT("\n");
			Current->Description = FText::FromString(Body + Line);
		}
		// 헤더가 없는 특수/미완성 무기 설명도 유실시키지 않습니다.
		else if (!Line.TrimStartAndEnd().IsEmpty())
		{
			Current = &Result.Sections.AddDefaulted_GetRef();
			Current->MainKeywordCode = NAME_None;
			Current->MainKeywordLabel = FText::GetEmpty();
			Current->BaseStats = BaseStats;
			Current->KeywordLabels = Labels;
			Current->Description = FText::FromString(Line);
		}
	}
	Result.Sections.StableSort([](const FCoinDescriptionSectionData& A, const FCoinDescriptionSectionData& B)
	{
		return static_cast<uint8>(A.MainKeyword) < static_cast<uint8>(B.MainKeyword);
	});
	return Result;
}

bool FCoinDescriptionFormatter::ResolveToken(FName Key, const FCoinDescriptionSectionData& Section, FCoinDescriptionTokenData& OutToken)
{
	OutToken = FCoinDescriptionTokenData();
	OutToken.TokenKey = Key;
	FString Type, Code;
	if (!Key.ToString().Split(TEXT(":"), &Type, &Code)) return false;
	if (Type == TEXT("KW"))
	{
		const FText* Label = Section.KeywordLabels.Find(FName(*Code));
		OutToken.Label = Label ? *Label : FText::FromString(Code);
		// 설명에서는 키워드도 Shift를 눌렀을 때만 이름을 표시합니다. 책갈피는 별도로 표시합니다.
		return true;
	}
	if (Type == TEXT("BUFF") && (Code == TEXT("Absorb") || Code == TEXT("Strike")))
	{
		OutToken.Label = FText::FromString(Code == TEXT("Absorb") ? TEXT("흡수버프") : TEXT("타격버프"));
		return true;
	}
	if (Type != TEXT("STAT")) return false;
	if (Code == TEXT("AttackRange") || Code == TEXT("AbilityRange"))
	{
		OutToken.Label = FText::FromString(Code == TEXT("AttackRange") ? TEXT("공격사거리") : TEXT("능력사거리"));
		return true;
	}
	OutToken.bHasValue = true;
	if (Code == TEXT("AttackPower"))
	{
		OutToken.Label = FText::FromString(TEXT("공격력"));
		OutToken.Value = Section.BaseStats.AttackPoint;
	}
	else if (Code == TEXT("WeaponPower"))
	{
		OutToken.Label = FText::FromString(TEXT("무기력"));
		OutToken.Value = Section.BaseStats.WeaponPoint;
	}
	else if (Code == TEXT("Count"))
	{
		OutToken.Label = FText::FromString(TEXT("횟수"));
		OutToken.Value = Section.BaseStats.WeaponCnt;
	}
	else return false;
	return true;
}

FCoinDescriptionInlineParts FCoinDescriptionFormatter::BuildInlineParts(const FCoinDescriptionTokenData& Data, bool bDetailed, bool bShowValue)
{
	FCoinDescriptionInlineParts Parts;
	const bool bHasValue = Data.bHasValue && bShowValue;
	if (bDetailed)
	{
		Parts.BeforeIcon = (bHasValue ? FString::FromInt(Data.Value) + TEXT(" ") : FString()) + TEXT("[");
		Parts.AfterIcon = TEXT("] (") + Data.Label.ToString() + TEXT(")");
	}
	else if (bHasValue)
	{
		Parts.AfterIcon = TEXT(" ") + FString::FromInt(Data.Value);
	}
	return Parts;
}

FString FCoinDescriptionFormatter::TokenText(const FCoinDescriptionTokenData& Token, bool bDetailed)
{
	// 일반 텍스트 변환에는 이미지를 삽입할 수 없으므로 아이콘 위치를 명시합니다.
	const FCoinDescriptionInlineParts Parts = BuildInlineParts(Token, bDetailed);
	return Parts.BeforeIcon + TEXT("<아이콘>") + Parts.AfterIcon;
}

FString FCoinDescriptionFormatter::ToRichText(const FString& RawText)
{
	const FString Text = CoinDescriptionFormatterPrivate::Normalize(RawText);
	FString Result;
	int32 Cursor = 0;
	while (Cursor < Text.Len())
	{
		const int32 Open = Text.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, Cursor);
		if (Open == INDEX_NONE) { Result += CoinDescriptionFormatterPrivate::Escape(Text.Mid(Cursor)); break; }
		Result += CoinDescriptionFormatterPrivate::Escape(Text.Mid(Cursor, Open - Cursor));
		const int32 Close = Text.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Open + 1);
		if (Close == INDEX_NONE) { Result += CoinDescriptionFormatterPrivate::Escape(Text.Mid(Open)); break; }
		const FString Key = Text.Mid(Open + 1, Close - Open - 1);
		FCoinDescriptionTokenData Token;
		if (Key.StartsWith(TEXT("VALUE:"))) Result += CoinDescriptionFormatterPrivate::Escape(Key.RightChop(6));
		else if (ResolveToken(FName(*Key), FCoinDescriptionSectionData(), Token))
			Result += FString::Printf(TEXT("<coin key=\"%s\"/>"), *CoinDescriptionFormatterPrivate::Escape(Key));
		else Result += CoinDescriptionFormatterPrivate::Escape(Text.Mid(Open, Close - Open + 1));
		Cursor = Close + 1;
	}
	return Result;
}

FString FCoinDescriptionFormatter::ToPlainText(const FString& RawText, const FCoinDescriptionSectionData& Section, bool bDetailed)
{
	FString Result = CoinDescriptionFormatterPrivate::Normalize(RawText);
	int32 Cursor = 0;
	while ((Cursor = Result.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, Cursor)) != INDEX_NONE)
	{
		const int32 End = Result.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Cursor + 1);
		if (End == INDEX_NONE) break;
		const FString Key = Result.Mid(Cursor + 1, End - Cursor - 1);
		FCoinDescriptionTokenData Token;
		FString Replacement;
		if (Key.StartsWith(TEXT("VALUE:"))) Replacement = Key.RightChop(6);
		else if (ResolveToken(FName(*Key), Section, Token)) Replacement = TokenText(Token, bDetailed);
		else { Cursor = End + 1; continue; }
		Result = Result.Left(Cursor) + Replacement + Result.Mid(End + 1);
		Cursor += Replacement.Len();
	}
	return Result;
}
