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
	Grid
};

UCLASS()
class FLIP_SIDE_API UUseableItemWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSelectItem> ItemSlotArray;

	struct FAttackAreaSpec ItemAreaSpec;

	UPROPERTY()
	class UItem_Action* SelectedItemAction;

	TArray<struct FGridPoint> OutCells;

	FGridPoint DefaultItemRange = {1, 1};

	bool bIsCoinSelectTurn = false;

	EUseableItemTargetMode CurrentTargetMode = EUseableItemTargetMode::None;

	UPROPERTY()
	AUseableItemActor* SelectedItemActor = nullptr;

/* Dependency post - managers */
protected:
	UPROPERTY()
	class UCoinManagementWSubsystem* CoinManager;

	UPROPERTY()
	class UGridManagerSubsystem* GridManager;

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

	/* Bind for item actor delegate */
	UFUNCTION() //hover
	void VisibleItemInfoUI(AUseableItemActor* TargetItem);

	UFUNCTION() //unhover
	void HideItemInfoUi();
		
	UFUNCTION() //click
	void SelectWantUseGridItem(AUseableItemActor* TargetItem);

	UFUNCTION() //click
	void SelectWantUseCoinItem(AUseableItemActor* TargetItem);
/* Execution */
protected:
	UFUNCTION()
	void ExecuteItemForGrid(class AGridActor* TargetGrid);

	UFUNCTION()
	void ExecuteItemForCoin(class ACoinActor* TargetCoin);

public:
	void SetItemInfo(AUseableItemActor* TargetItem);

	void CancelWantUseItem();

	//BattleManager call when CoinSelectTurn
	void CoinBindsToItemMan();

	//ItemAction의 ItemType과 같은 이유
	//bool IsItemSetupInGrid() const;

	void SetTurn(const bool bIsTurn);
};
