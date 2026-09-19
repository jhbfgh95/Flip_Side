#pragma once

#include "CoreMinimal.h"
#include "DataTypes/CoinDescriptionDataTypes.h"

// 수식의 원래 순서/괄호를 보관합니다. Key가 없으면 숫자 또는 연산자입니다.
struct FCoinExpressionPart
{
	FName Key;
	FString Text;
};

struct FCoinExpressionResult
{
	bool bValid = false;
	bool bHasUnknownValue = false;
	double Value = 0.0;
	double DisplayValue = 0.0;
	FString Error;
	TArray<FName> UniqueKeys;
	TArray<FCoinExpressionPart> Parts;
};

// UI 전용 계산입니다. 코인 슬롯의 기본 스탯만 사용하며 행동/DB 상태를 변경하지 않습니다.
class FLIP_SIDE_API FCoinDescriptionExpression
{
public:
	static FCoinExpressionResult Evaluate(const FString& Expression, const FCoinDescriptionSectionData& Section);
};
