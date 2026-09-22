#pragma once
#include "CoreMinimal.h"
#include "DebuffDataTypes.generated.h"

/** DB 표시 전용 정의. 적용 수치/남은 턴/출처는 DebuffComponent에서 관리합니다. */
USTRUCT(BlueprintType)
struct FDebuffDefinitionData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BuffTypeID = INDEX_NONE;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UTexture2D> Icon = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;
};
