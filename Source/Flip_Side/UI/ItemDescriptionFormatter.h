#pragma once

#include "CoreMinimal.h"

struct FItemDescriptionDisplay
{
	FString Header;
	FName HeaderKeyword = NAME_None;
	FString Body;
	FString PlainText;
};

// 공용 DB 원문은 그대로 두고 전투 팝업에서만 RichText로 변환합니다.
class FLIP_SIDE_API FItemDescriptionFormatter
{
public:
	static FItemDescriptionDisplay Format(const FString& Raw);
	static FName FindKeyword(const FString& Label);
};
