#pragma once

#include "CoreMinimal.h"
#include "DataTypes/CoinStatDataTypes.h"
#include "CoinDescriptionDataTypes.generated.h"

UENUM(BlueprintType)
enum class ECoinDescriptionKeyword : uint8
{
	Attack,
	Mobility,
	Hit
};

/** DB 설명을 나눈 한 구간입니다. 스탯은 슬롯 무기의 기본값만 보유합니다. */
USTRUCT(BlueprintType)
struct FCoinDescriptionSectionData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECoinDescriptionKeyword MainKeyword = ECoinDescriptionKeyword::Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MainKeywordCode = TEXT("Attack");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText MainKeywordLabel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> AdditionalKeywords;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> AdditionalKeywordCodes;
	// 예: : [STAT:Count]. 반복 판정이 아닌 표시용 문자열입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HeaderSuffix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponNumericStats BaseStats;
	// 이름만 DB에서 전달하며, 색상·아이콘·크기는 BP가 소유합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FText> KeywordLabels;
};

USTRUCT(BlueprintType)
struct FCoinWeaponDescriptionData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCoinDescriptionSectionData> Sections;
};

/** DB 문장 식별자를 이름/기본 수치로 해석한 데이터입니다. 별도 토큰 위젯은 생성하지 않습니다. */
USTRUCT(BlueprintType)
struct FCoinDescriptionTokenData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FName TokenKey;
	UPROPERTY(BlueprintReadOnly)
	FText Label;
	UPROPERTY(BlueprintReadOnly)
	int32 Value = 0;
	UPROPERTY(BlueprintReadOnly)
	bool bHasValue = false;
};
