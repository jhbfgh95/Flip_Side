// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipSide_Enum.h"
#include "NiagaraSystem.h"
#include "ItemDataTypes.generated.h"

UENUM(BlueprintType)
enum class EItemVFXTarget : uint8
{
    None,
    TargetGrid,
    TargetCoin,
    TargetOther
};

USTRUCT(BlueprintType)
struct FItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemTypeID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EItemType ItemType = EItemType::CoinBuff;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemRange = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemEffectValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Item_DES;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* ItemIcon=nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TypeColor = FLinearColor::White;

    // DB���� ���� �ڵ� ���ڿ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BehaviorCode;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Price = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UNiagaraSystem> ItemVFX = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EItemVFXTarget ItemVFXTarget = EItemVFXTarget::None;
};

/**
 * ������ ���� ��� ����
 */
USTRUCT(BlueprintType)
struct FSelectItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SameItemNum = 0;
};
