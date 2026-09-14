// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KeywordDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FKeywordDefinitionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 KeywordID = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    FName KeywordCode = NAME_None;

    UPROPERTY(BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(BlueprintReadOnly)
    FText Description;

    UPROPERTY(BlueprintReadOnly)
    FLinearColor UIColor = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly)
    bool bEnabled = false;

    UPROPERTY(BlueprintReadOnly)
    int32 SortOrder = 0;
};
