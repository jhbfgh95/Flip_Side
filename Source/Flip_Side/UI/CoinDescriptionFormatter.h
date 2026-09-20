#pragma once

#include "CoreMinimal.h"
#include "DataTypes/CoinDescriptionDataTypes.h"
#include "DataTypes/KeywordDataTypes.h"

// 아이콘 전후의 실제 문자입니다. 대괄호는 Shift 상세 표시에서만 출력합니다.
struct FCoinDescriptionInlineParts
{
	FString BeforeIcon;
	FString AfterIcon;
};

/** DB 읽기/전투 계산 없이, Controller가 전달한 설명을 표시 데이터로 변환합니다. */
class FLIP_SIDE_API FCoinDescriptionFormatter
{
public:
	static FCoinDescriptionInlineParts BuildInlineParts(const FCoinDescriptionTokenData& Data, bool bDetailed, bool bShowValue = true);
	static FCoinWeaponDescriptionData Parse(const FString& RawDescription,
		const TArray<FKeywordDefinitionData>& Keywords, const FWeaponNumericStats& BaseStats);
	static FString ToRichText(const FString& RawText);
	static FString ToPlainText(const FString& RawText, const FCoinDescriptionSectionData& Section, bool bDetailed);
	static bool ResolveToken(FName Key, const FCoinDescriptionSectionData& Section, FCoinDescriptionTokenData& OutToken);
	static FString TokenText(const FCoinDescriptionTokenData& Token, bool bDetailed);
};
