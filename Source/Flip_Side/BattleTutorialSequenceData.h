// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
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

USTRUCT(BlueprintType)
struct FBattleTutorialStep : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "1", ToolTip = "작은 번호부터 진행합니다. DT 화면의 정렬 순서와는 별개입니다."))
	int32 Order = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	EBattleTutorialTargetType TargetType = EBattleTutorialTargetType::Widget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (GetOptions = "/Script/Flip_Side.BattleTutorialSequenceData:GetWidgetHierarchyOptionsForTable"))
	FString WidgetPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (GetOptions = "/Script/Flip_Side.BattleTutorialSequenceData:GetWidgetHierarchyOptionsForTable"))
	FString ActionWidgetPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ToolTip = "X: 좌우 각각, Y: 상하 각각의 여백. 양수는 바깥으로 확장하고 음수는 안쪽으로 줄입니다. UMG 좌표 단위입니다."))
	FVector2D Padding = FVector2D(12.f, 12.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (ClampMin = "0", DisplayName = "Explanation Layout Index", ToolTip = "Overlay의 ExplanationLayouts 배열 번호입니다. 같은 번호의 위치와 크기를 함께 사용합니다. 기존 ExplanationPositionIndex 데이터는 그대로 유지됩니다."))
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Transition", meta = (ClampMin = "0", Units = "s", DisplayName = "Next Step Delay (Seconds)", ToolTip = "현재 단계의 클릭, 호버 또는 연출 완료 조건을 만족한 뒤 다음 단계로 넘어가기 전 기다리는 시간입니다. 0초는 기존처럼 즉시 진행합니다."))
	float NextStepDelay = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Transition")
	bool bCloseCoinInfoOnExit = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Transition")
	bool bCloseItemInfoOnExit = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Transition")
	bool bReturnToReadyOnExit = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Transition")
	bool bRunBossPatternOnExit = false;

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

	UFUNCTION(CallInEditor)
	static TArray<FString> GetWidgetHierarchyOptionsForTable();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (RequiredAssetDataTags = "RowStructure=/Script/Flip_Side.BattleTutorialStep", ToolTip = "단계별 설명과 진행 조건을 수정할 DT입니다. 각 행의 Order 순서로 진행합니다."))
	TObjectPtr<UDataTable> StepTable;

	UFUNCTION(BlueprintPure, Category = "Tutorial")
	TArray<FBattleTutorialStep> GetOrderedSteps() const;

#if WITH_EDITOR
	// 로컬 에디터 도구에서 원본 구조체를 직접 복사하여 FText와 모든 필드를 보존합니다.
	UFUNCTION()
	bool CopyLegacyStepsToTable(UDataTable* InTable);
#endif

private:
	// 기존 DA 데이터는 보존하고, 편집 및 진행에는 StepTable을 사용합니다.
	UPROPERTY()
	TArray<FBattleTutorialStep> Steps;
};
