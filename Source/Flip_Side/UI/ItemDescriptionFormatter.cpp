#include "UI/ItemDescriptionFormatter.h"

namespace ItemDescriptionPrivate
{
	FString Escape(const FString& Text)
	{
		return Text.Replace(TEXT("&"), TEXT("&amp;")).Replace(TEXT("<"), TEXT("&lt;"))
			.Replace(TEXT(">"), TEXT("&gt;")).Replace(TEXT("\""), TEXT("&quot;"));
	}
	FString Inline(const FString& Text)
	{
		FString Result;
		int32 Cursor = 0;
		while (Cursor < Text.Len())
		{
			const int32 Open = Text.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, Cursor);
			if (Open == INDEX_NONE) { Result += Escape(Text.Mid(Cursor)); break; }
			Result += Escape(Text.Mid(Cursor, Open - Cursor));
			const int32 Close = Text.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Open + 1);
			if (Close == INDEX_NONE) { Result += Escape(Text.Mid(Open)); break; }
			const FName Code = FItemDescriptionFormatter::FindKeyword(Text.Mid(Open + 1, Close - Open - 1));
			if (!Code.IsNone()) Result += FString::Printf(TEXT("<itemkw code=\"%s\"/>"), *Code.ToString());
			else Result += Escape(Text.Mid(Open, Close - Open + 1));
			Cursor = Close + 1;
		}
		return Result;
	}
}

FName FItemDescriptionFormatter::FindKeyword(const FString& Label)
{
	if (Label == TEXT("공격") || Label == TEXT("KW:Attack")) return TEXT("Attack");
	if (Label == TEXT("적중") || Label == TEXT("KW:Hit")) return TEXT("Hit");
	if (Label == TEXT("즉시") || Label == TEXT("KW:Instant")) return TEXT("Instant");
	return NAME_None;
}

FItemDescriptionDisplay FItemDescriptionFormatter::Format(const FString& Raw)
{
	FItemDescriptionDisplay Result;
	// 기존 DB의 이중 escape도 호환하되, 새 DB에는 실제 줄바꿈을 저장합니다.
	Result.PlainText = Raw.Replace(TEXT("\\\\n"), TEXT("\n")).Replace(TEXT("\\n"), TEXT("\n"))
		.Replace(TEXT("\r\n"), TEXT("\n")).Replace(TEXT("\r"), TEXT("\n"));
	TArray<FString> Lines, Body;
	Result.PlainText.ParseIntoArrayLines(Lines, false);
	for (const FString& Line : Lines)
	{
		const FString Trimmed = Line.TrimStartAndEnd();
		// 헤더는 키워드만 있는 행입니다. 본문 속 [공격]은 본문에 남깁니다.
		if (Trimmed.StartsWith(TEXT("[")) && Trimmed.EndsWith(TEXT("]")) &&
			!FindKeyword(Trimmed.Mid(1, Trimmed.Len() - 2)).IsNone())
		{
			if (Result.HeaderKeyword.IsNone()) Result.HeaderKeyword = FindKeyword(Trimmed.Mid(1, Trimmed.Len() - 2));
			if (!Result.Header.IsEmpty()) Result.Header += TEXT(" ");
			Result.Header += ItemDescriptionPrivate::Inline(Trimmed);
			continue;
		}
		// 문서의 들여쓰기와 본문을 유지합니다. 제목용 DataTable 태그는 생성하지 않습니다.
		Body.Add(ItemDescriptionPrivate::Inline(Line));
	}
	Result.Body = FString::Join(Body, TEXT("\n"));
	return Result;
}
