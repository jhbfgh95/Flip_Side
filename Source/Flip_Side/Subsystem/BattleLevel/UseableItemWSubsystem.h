#pragma once

#include "CoreMinimal.h"
#include "ItemDataTypes.h"
#include "AttackAreaTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "UseableItemWSubsystem.generated.h"

class AUseableItemActor;

UCLASS()
class FLIP_SIDE_API UUseableItemWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSelectItem> ItemSlotArray;

	struct FAttackAreaSpec ItemAreaSpec;

	class UItem_Action* SelectedItemAction;

	TArray<struct FGridPoint> OutCells;

	FGridPoint DefaultItemRange = {1, 1};

	bool bIsCoinSelectTurn = false;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	void InitUseitemSlot();

	void InstanceUseItems();

	void InitSelectedItem();

	void ApplyRangedThings();
public:
	void SelectWantUseItem(AUseableItemActor* TargetItem);

	void CancelWantUseItem();

	void ExecuteItem(class AGridActor* TargetGrid);

	bool IsItemSetupInGrid() const;

	void SetTurn(const bool bIsTurn) { bIsCoinSelectTurn = bIsTurn; }
};
