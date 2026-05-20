// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "FlipSide_Enum.h"
#include "Engine/Texture2D.h"
#include "NiagaraSystem.h"
#include "AttackAreaTypes.h"
#include "Sound/SoundBase.h"
class UBehavior; // forward declaration
#include "WeaponDataTypes.generated.h"

struct FWeaponActionResolveResult
{
    bool bCanExecute = false;
    bool bDamagesBoss = false;
    bool bAppliesBossCC = false;

    TWeakObjectPtr<class ABossActor> Boss = nullptr;
};

USTRUCT(BlueprintType)
struct FFaceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 WeaponID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 TypeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 HP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWeaponClass WeaponType = EWeaponClass::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackPoint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BehaviorPoint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGridPoint AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* WeaponIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BehaviorCode;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString KOR_DES;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ENG_DES;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UNiagaraSystem* WeaponVFX = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWeaponVFXTarget WeaponVFXTarget = EWeaponVFXTarget::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TypeColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FAttackAreaSpec AttackAreaSpec;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FIntPoint AttackAnchorOffset = FIntPoint(0, 0);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AttackAreaFlags = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EActionRepeatType ActionRepeatType = EActionRepeatType::None;

    // pattern == SingleCell 일 때만 유효 (param_b 값)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ESingleCellWeaponType SingleCellWeaponType = ESingleCellWeaponType::Other;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Price = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* WeaponSFX = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponType
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 TypeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWeaponClass WeaponType = EWeaponClass::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 HP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TypeColor = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct FWeaponIDArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<int32> WeaponIDs;
};


/**
 * �ൿ �Ͽ��� ����� �۾� ����
 */
USTRUCT(BlueprintType)
struct FActionTask
{
    GENERATED_BODY()

    FGridPoint ModifiedRange;

    int32 ModifiedAttackPoint = 0;

    int32 ModifiedBehaviorPoint = 0;
};

