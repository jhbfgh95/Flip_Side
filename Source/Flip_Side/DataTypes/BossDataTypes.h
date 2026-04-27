#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "BossDataTypes.generated.h"

class ABossActor;

USTRUCT(BlueprintType)
struct FBossData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossStage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ThemeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BossName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> BossImage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText BossAbilityDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackPoint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector SpawnLoc = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator SpawnRot = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BossHP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<ABossActor> BossClass;
};
