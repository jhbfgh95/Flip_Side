// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BattleTutorialSequenceData.generated.h"

UENUM(BlueprintType)
enum class EBattleTutorialAdvanceType : uint8
{
	OverlayClick,
	CoinSlotClick,
	LeverClick,
	End
};

USTRUCT(BlueprintType)
struct FBattleTutorialStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (MultiLine = "true"))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FName FocusId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FVector2D HoleSize = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	EBattleTutorialAdvanceType AdvanceType = EBattleTutorialAdvanceType::OverlayClick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "1"))
	int32 RequiredClickCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	bool bUIOnly = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	bool bUseTopTextBox = false;
};

UCLASS(BlueprintType)
class FLIP_SIDE_API UBattleTutorialSequenceData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	TArray<FBattleTutorialStep> Steps;
};
