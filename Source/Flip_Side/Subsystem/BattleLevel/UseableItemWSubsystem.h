#pragma once

#include "CoreMinimal.h"
#include "ItemDataTypes.h"
#include "AttackAreaTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "UseableItemWSubsystem.generated.h"

class AUseableItemActor;

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
	TArray<AUseableItemActor*> UnUsedItemArray;

	struct FAttackAreaSpec ItemAreaSpec;

	UPROPERTY()
	class UItem_Action* SelectedItemAction;

	TArray<struct FGridPoint> OutCells;

	FGridPoint DefaultItemRange = {1, 1};

	bool bIsCoinSelectTurn = false;

	EUseableItemTargetMode CurrentTargetMode = EUseableItemTargetMode::None;

	UPROPERTY()
	AUseableItemActor* SelectedItemActor = nullptr;

	UPROPERTY()
	FItemData SelectedItemData;

	bool bHasSelectedItemData = false;

	UPROPERTY()
	class ACoinActor* SelectedTargetCoin = nullptr;

	UPROPERTY()
	TArray<FGridPoint> ValidTargetGrids;

	UPROPERTY()
	class AItemPreviewActor* ItemPreviewActor = nullptr;

	FTimerHandle ItemPreviewFollowTimerHandle;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Preview")
	float ItemPreviewPlaneZ = -80.0f;

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

	/*UI for Hovering */
	UPROPERTY()
    class UW_ItemInfo* ItemInfoWidgetInstance = nullptr;
	

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	void InitUseitemSlot();

	void InstanceUseItems();

	void InitSelectedItem();

	void ApplyRangedThings();

	void BindItemDelegates(AUseableItemActor* TargetItem);

	bool TryGetItemData(AUseableItemActor* TargetItem, FItemData& OutItemData);

	void ConsumeSelectedItemActor();

	void ConsumeSelectedItemActorOnly();

	void PlayItemFailedFeedback();

	void PlaySelectedItemSuccessVFX(class AGridActor* TargetGrid, class ACoinActor* TargetCoin, AActor* TargetOther);

	void StartItemCursorPreview(AUseableItemActor* SourceItem);

	void UpdateItemCursorPreview();

	void StopItemCursorPreview();

	/* Bind for item actor delegate */
	UFUNCTION() //hover
	void VisibleItemInfoUI(AUseableItemActor* TargetItem);

	UFUNCTION() //unhover
	void HideItemInfoUi();
		
	UFUNCTION() //click
	void SelectWantUseGridItem(AUseableItemActor* TargetItem);

	UFUNCTION() //click
	void SelectWantUseCoinItem(AUseableItemActor* TargetItem);

	UFUNCTION() //RightClick
	void HandleItemRightClicked(AUseableItemActor* TargetItem);

	UFUNCTION()
	void HandleBattleCoinHoveredForItem(class ACoinActor* HoveredCoin);

	UFUNCTION()
	void HandleBattleCoinUnhoveredForItem();
/* Execution */
protected:
	UFUNCTION()
	void ExecuteItemForGrid(class AGridActor* TargetGrid);

	UFUNCTION()
	void ExecuteItemForCoin(class ACoinActor* TargetCoin);

public:
	void SetItemInfo(AUseableItemActor* TargetItem);

	bool HasPendingItemSelection() const;

	bool TryCancelCurrentItemSelection();

	void CancelWantUseItem();

	void BuildEverywhereValidTargetGrids(class ACoinActor* TargetCoin);

	//BattleManager call when CoinSelectTurn
	void CoinBindsToItemMan();

	//ItemAction의 ItemType과 같은 이유
	//bool IsItemSetupInGrid() const;

	void SetTurn(const bool bIsTurn);

	int32 CalculateItemPrice() const;

	int32 CalculateItemCount() const;
};
