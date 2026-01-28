// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardTypes.generated.h"

USTRUCT(BlueprintType)
struct FCardData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CardID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString CardName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Card_Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Icon = nullptr;
};