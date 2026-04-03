#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridTypes.h"
#include "AttackAreaTypes.h"
#include "WeaponRangePreviewActor.generated.h"

class AGridActor;

/**
 * 레벨 내 별도 위치에 5x8 그리드 2개를 스폰.
 * - 왼쪽 그리드: 앞면 무기 사거리
 * - 오른쪽 그리드: 뒷면 무기 사거리
 *
 * HandleCoinSlotHovered에서 ShowBothFacePreview() 호출.
 */
UCLASS()
class FLIP_SIDE_API AWeaponRangePreviewActor : public AActor
{
	GENERATED_BODY()

public:
	AWeaponRangePreviewActor();

protected:
	virtual void BeginPlay() override;

public:
	// 앞면 그리드 원점
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	FVector FrontGridOrigin = FVector(100000.f, 0.f, 0.f);

	// 뒷면 그리드 원점
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	FVector BackGridOrigin = FVector(100000.f, -4000.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	float SpacingX = 440.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	float SpacingY = 440.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	int32 GridXSize = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	int32 GridYSize = 5;

	// 에디터에서 BP_GridActor 할당
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	TSubclassOf<AGridActor> GridActorClass;

	// 코인이 올라갈 기준 셀 (두 그리드 모두 동일)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Grid")
	FGridPoint CoinCell = FGridPoint{ 3, 2 };

	// 사거리 하이라이트 색
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Visual")
	FLinearColor RangeHighlightColor = FLinearColor(1.f, 0.5f, 0.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Visual")
	float RangeFillIntensity = 0.8f;

	// 코인 위치 셀 색
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Visual")
	FLinearColor CoinCellColor = FLinearColor(0.f, 1.f, 0.3f, 1.f);

	// 기본 셀 색
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Visual")
	FLinearColor DefaultCellColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview|Visual")
	float DefaultFillIntensity = 0.4f;

	/** 앞면/뒷면 사거리 동시 표시. HandleCoinSlotHovered에서 호출 */
	UFUNCTION(BlueprintCallable, Category = "Preview")
	void ShowBothFacePreview(const FAttackAreaSpec& FrontSpec, const FAttackAreaSpec& BackSpec);

	/** 미리보기 초기화 */
	UFUNCTION(BlueprintCallable, Category = "Preview")
	void ClearPreview();

	/**
	 * GridManagerSubsystem이 계산한 RangeCells를 받아서 자신의 그리드에 표시.
	 * 직접 호출하지 말고 UGridManagerSubsystem::PreviewHoveredCoinRange를 사용할 것.
	 */
	UFUNCTION(BlueprintCallable, Category = "Preview")
	void ShowPreview(const FGridPoint& CoinXY, const TArray<FGridPoint>& RangeCells);

	/** 그리드 재생성 */
	UFUNCTION(BlueprintCallable, Category = "Preview")
	void RebuildGrid();

private:
	void SpawnGrids();
	void DestroyGrids();

	void ApplyPreviewToGrid(
		TMap<FGridPoint, TObjectPtr<AGridActor>>& GridMap,
		const FAttackAreaSpec& Spec,
		TArray<FGridPoint>& OutHighlightedCells
	);

	void ClearGridHighlight(
		TMap<FGridPoint, TObjectPtr<AGridActor>>& GridMap,
		TArray<FGridPoint>& HighlightedCells
	);

	AGridActor* GetGridActorAt(TMap<FGridPoint, TObjectPtr<AGridActor>>& GridMap, int32 X, int32 Y) const;
	bool IsInGrid(int32 X, int32 Y) const;

	// 앞면 그리드
	UPROPERTY()
	TMap<FGridPoint, TObjectPtr<AGridActor>> FrontGridActors;

	// 뒷면 그리드
	UPROPERTY()
	TMap<FGridPoint, TObjectPtr<AGridActor>> BackGridActors;

	TArray<FGridPoint> FrontHighlightedCells;
	TArray<FGridPoint> BackHighlightedCells;
};
