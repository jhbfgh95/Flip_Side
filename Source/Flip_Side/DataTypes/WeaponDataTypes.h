// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "FlipSide_Enum.h"
#include "Engine/Texture2D.h"
#include "NiagaraSystem.h"
class UBehavior; // forward declaration
#include "WeaponDataTypes.generated.h"

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

    // DB에서 읽은 코드 문자열
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BehaviorCode;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString KOR_DES;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ENG_DES;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UNiagaraSystem* WeaponVFX = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TypeColor;
};


struct FWeaponType
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 WeaponID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWeaponClass WeaponType = EWeaponClass::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 HP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TypeColor;
};

USTRUCT(BlueprintType)
struct FWeaponIDArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<int32> WeaponIDs;
};


/**
 * 행동 턴에서 실행될 작업 단위
 */
USTRUCT(BlueprintType)
struct FBehaviorTask
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CoinID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SelectedWeaponID = 0;

    // 실제 실행될 행동 객체
    /*
    UPROPERTY()
    TObjectPtr<UBehavior> Behavior = nullptr;
    */
};

