#pragma once


#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "AttackAreaTypes.h"
#include "BossActor.h"
#include "GridManagerSubsystem.generated.h"

class AGridActor;
class ACoinActor;
class AWeaponRangePreviewActor;

USTRUCT(BlueprintType)
struct FCoinOnGridInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 CoinID = 0;
	UPROPERTY(BlueprintReadOnly) FGridPoint GridXY;
	ACoinActor* CoinActor = nullptr;
};

//Park Added
USTRUCT(BlueprintType)
struct FObjectOnGridInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> Coins;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABossActor> Boss = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> Others;
};

USTRUCT()
struct FCellDoorFxState
{
	GENERATED_BODY()

public:
	FTimerHandle Phase1Tick;
	FTimerHandle Phase2Tick;

	float PhaseDuration = 1.5f;

	float Phase1StartTime = 0.f;
	float Phase2StartTime = 0.f;

	FSimpleDelegate CompletionDelegate;
};

UENUM(BlueprintType)
enum class EGridClickFlag : uint8
{
	None,
	CoinAction,
	ItemAction
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGridClickedForCoin, AGridActor*, targetGrid);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGridClickedForItem, AGridActor*,  targetGrid);

UCLASS()
class FLIP_SIDE_API UGridManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	FVector GridOrigin = FVector(1340.f, -2560.f, -100.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	float SpacingX = 440.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	float SpacingY = 440.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	int32 GridXSize = 8; // ����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Spawn")
	int32 GridYSize = 5; // ����

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void InitGrid(int32 InGridXSize, int32 InGridYSize);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void ClearGrid();

	UFUNCTION(BlueprintCallable)
	void PlaySingleCellDoorOpenFx(int32 GridX, int32 GridY, float PhaseDuration = 1.5f);

	/** C++ 진입 연출에서 실제 문 닫힘 완료를 집계할 때 사용합니다. */
	bool PlaySingleCellDoorOpenFxTracked(int32 GridX, int32 GridY, float PhaseDuration, FSimpleDelegate OnFinished);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CollectOccupiedCoins(TArray<FCoinOnGridInfo>& OutCoins) const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void GetObjectsAtRange(const FAttackAreaSpec& Spec, const FGridPoint& FinalRange, TArray<FGridPoint>& OutCells, FObjectOnGridInfo& Infos) const;

	UFUNCTION(BlueprintCallable, Category = "Grid|Boss")
	void BuildBossAttackCells(const FAttackAreaSpec& Spec, TArray<FGridPoint>& OutCells) const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	AGridActor* GetGridActor(const FGridPoint& P) const;

	/** 보스가 사용할 수 있는 뒤쪽 9x3 영역입니다. 9x9 기준 X=0~8, Y=6~8입니다. */
	UFUNCTION(BlueprintPure, Category = "Grid|Boss")
	bool IsBossAreaCell(const FGridPoint& P) const;

	/** CoinActor가 현재 해당 셀을 새로 점유할 수 있는지 검사합니다. */
	UFUNCTION(BlueprintPure, Category = "Grid|Coin")
	bool CanCoinOccupyCell(const FGridPoint& P) const;

	bool TryOccupyCoinCell(const FGridPoint& P, ACoinActor* Coin);
	void ReleaseCoinCell(const FGridPoint& P, ACoinActor* Coin);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void InitGrids();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void GetValidGridsForSingleCell(const FGridPoint& CoinXY, const FAttackAreaSpec& Spec, TArray<FGridPoint>& VadlidCells);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void PreviewHoveredCoinRange(const FGridPoint& CoinXY, const FAttackAreaSpec& Spec, const FGridPoint& finalRange);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void ResetBattleCoinPreview();

	// WeaponRangePreviewActor가 BeginPlay에서 자신을 등록
	void RegisterPreviewActor(AWeaponRangePreviewActor* Actor) { PreviewActor = Actor; }

	using FCoinPred = TFunctionRef<bool(const FCoinOnGridInfo&)>;

	void BuildCoinTargetCells(
		FCoinPred Predicate,
		TArray<FGridPoint>& OutCells
	) const;

/* 클릭 함수 및 델리게이트 변수들 */
public:
	void SetGridClickFlag(EGridClickFlag clickFlag = EGridClickFlag::None, bool bUpdateHoverFlags = true);

	void SetGridItemFlags(int32 InItemFlag);

	void SetGridHoverFlags(int32 InHoverFlag);

	void SetGridItemTargetGrids(const TArray<FGridPoint>& TargetGrids);

	FOnGridClickedForCoin OnGridClickedForCoin;
	FOnGridClickedForItem OnGridClickedForItem;
private:
	void InstanceGrid();

	UPROPERTY(Transient)
	TMap<FGridPoint, FCellDoorFxState> DoorFxByCell;

	bool IsInGrid(int32 X, int32 Y) const;

	AGridActor* GetGridActorAt(int32 X, int32 Y) const;

	bool StartSingleCellDoorOpenFx(int32 GridX, int32 GridY, float PhaseDuration, FSimpleDelegate OnFinished);
	void StopDoorFx(const FGridPoint& Cell, bool bNotifyCompletion = false);

	void TickPhase1(FGridPoint Cell);
	void StartPhase2(FGridPoint Cell);
	void TickPhase2(FGridPoint Cell);

/* Click Members */
	EGridClickFlag ClickFlag = EGridClickFlag::None;

	UFUNCTION()
	void BindForGridClick(AGridActor* targetGrid);

private:
	UPROPERTY()
	TMap<FGridPoint, TObjectPtr<AGridActor>> GridActors;

	UPROPERTY()
	TObjectPtr<AWeaponRangePreviewActor> PreviewActor = nullptr;

	// 메인 그리드 미리보기 상태 추적 (ResetBattleCoinPreview용)
	TArray<FGridPoint> MainPreviewHighlightedCells;
	FGridPoint MainPreviewCoinCell = FGridPoint{ -1, -1 };
};
