// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Blueprint/UserWidget.h"
#include "BattleTutorialSequenceData.generated.h"

UENUM(BlueprintType)
enum class EBattleTutorialAdvanceType : uint8
{
	OverlayClick,
	CoinSlotClick,
	LeverClick,
	End,
	TargetAction,
	CoinAction,
	ItemSelected,
	ItemUsed,
	CoinActionStarted
};

UENUM(BlueprintType)
enum class EBattleTutorialTargetType : uint8 { None, Widget, Actor };

UENUM(BlueprintType)
enum class EBattleTutorialFrame : uint8 { Auto, Landscape4To3, Square, Portrait3To4 };

USTRUCT(BlueprintType)
struct FBattleTutorialStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	EBattleTutorialTargetType TargetType = EBattleTutorialTargetType::Widget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (GetOptions = "GetWidgetHierarchyOptions"))
	FString WidgetPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (GetOptions = "GetWidgetHierarchyOptions"))
	FString ActionWidgetPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "0", ToolTip = "X: 좌우 여백, Y: 상하 여백. UMG 좌표 단위입니다."))
	FVector2D Padding = FVector2D(12.f, 12.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	EBattleTutorialFrame Frame = EBattleTutorialFrame::Auto;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "0"))
	int32 ExplanationPositionIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	bool bRequireHighlightedAction = false;

	// HUD 코인 슬롯은 1부터 시작합니다. 0이면 모든 슬롯의 성공 동작을 받습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "0"))
	int32 CoinSlotNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	int32 ItemID = -1;

	// 0: 프로모션 쇠파이프, 1: 뒤쪽 쇠파이프, 2: 조준 렌즈, 3: 응급처치 키트.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "-1", ClampMax = "3"))
	int32 RuntimeCoinIndex = -1;

	// 강조 대상과 행동을 완료하는 코인이 다를 때 지정합니다. -1이면 RuntimeCoinIndex를 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "-1", ClampMax = "3"))
	int32 ActionRuntimeCoinIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FName ActionId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FName FocusId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FVector2D HoleSize = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	EBattleTutorialAdvanceType AdvanceType = EBattleTutorialAdvanceType::OverlayClick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "1"))
	int32 RequiredClickCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (AdvancedDisplay, ToolTip = "이전 데이터 호환용. 입력은 강조 영역과 진행 조건으로 제어합니다."))
	bool bUIOnly = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (AdvancedDisplay, ToolTip = "이전 데이터 호환용. ExplanationPositionIndex를 사용합니다."))
	bool bUseTopTextBox = false;
};

UCLASS(BlueprintType)
class FLIP_SIDE_API UBattleTutorialSequenceData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	TSubclassOf<UUserWidget> TargetHUDClass;

	UFUNCTION(CallInEditor)
	TArray<FString> GetWidgetHierarchyOptions() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	TArray<FBattleTutorialStep> Steps;
};
