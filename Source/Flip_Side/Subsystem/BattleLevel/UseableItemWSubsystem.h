#pragma once

#include "CoreMinimal.h"
#include "ItemDataTypes.h"
#include "AttackAreaTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "UseableItemWSubsystem.generated.h"

class AUseableItemActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleItemDataChanged);

UENUM()
enum class EUseableItemTargetMode : uint8
{
	None,
	Coin,
	CoinThenGrid,
	Grid
};

UCLASS()
class FLIP_SIDE_API UUseableItemWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSelectItem> ItemSlotArray;

	UPROPERTY()
	TArray<FBattleItemSlotData> BattleItemSlots;

	struct FAttackAreaSpec ItemAreaSpec;

	UPROPERTY()
	class UItem_Action* SelectedItemAction;

	TArray<struct FGridPoint> OutCells;

	FGridPoint DefaultItemRange = {1, 1};

	bool bIsCoinBehaviorTurn = false;

	EUseableItemTargetMode CurrentTargetMode = EUseableItemTargetMode::None;

	int32 SelectedItemID = INDEX_NONE;

	UPROPERTY()
	FItemData SelectedItemData;

	bool bHasSelectedItemData = false;

	UPROPERTY()
	class ACoinActor* SelectedTargetCoin = nullptr;

	UPROPERTY()
	TArray<FGridPoint> ValidTargetGrids;

	UPROPERTY()
	class AUseableItemActor* PreviewItemActor = nullptr;

	FTimerHandle ItemPreviewFollowTimerHandle;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Preview")
	float ItemPreviewPlaneZ = -80.0f;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Preview")
	float ItemPreviewScaleMultiplier = 0.6f;

/* Dependency post - managers */
protected:
	UPROPERTY()
	class UCoinActionManagementWSubsystem* CoinActionManager;

	UPROPERTY()
	class UCoinManagementWSubsystem* CoinManager;

	UPROPERTY()
	class UGridManagerSubsystem* GridManager;

	UPROPERTY()
	class UBattleLevelActingWSubsystem* ActingManager;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	void InitUseitemSlot();

	void InitializeBattleItemSlots();

	void InitSelectedItem();

	void ApplyRangedThings();

	FBattleItemSlotData* FindBattleItemSlot(int32 ItemID);

	const FBattleItemSlotData* FindBattleItemSlot(int32 ItemID) const;

	bool SetItemInfo(int32 ItemID);

	void ConsumeSelectedItem();

	void ConsumeSelectedItemOnly();

	void PlayItemFailedFeedback();

	void PlaySelectedItemSuccessVFX(class AGridActor* TargetGrid, class ACoinActor* TargetCoin, AActor* TargetOther);

	void StartItemCursorPreview(const FItemData& SourceItemData);

	void UpdateItemCursorPreview();

	void StopItemCursorPreview();

/* Execution */
protected:
	UFUNCTION()
	void ExecuteItemForGrid(class AGridActor* TargetGrid);

	UFUNCTION()
	void ExecuteItemForCoin(class ACoinActor* TargetCoin);

public:
	UPROPERTY(BlueprintAssignable, Category = "UseableItem")
	FOnBattleItemDataChanged OnBattleItemDataChanged;

	const TArray<FBattleItemSlotData>& GetBattleItemSlots() const { return BattleItemSlots; }

	bool IsItemUseAvailable() const;

	bool TrySelectItem(int32 ItemID);

	void CancelWantUseItem();

	void BuildEverywhereValidTargetGrids(class ACoinActor* TargetCoin);

	//BattleManager call when CoinBehaviorTurn
	void CoinBindsToItemMan();

	//ItemAction의 ItemType과 같은 이유
	//bool IsItemSetupInGrid() const;

	void SetTurn(const bool bIsTurn);

	int32 CalculateItemPrice() const;

	int32 CalculateItemCount() const;
};
