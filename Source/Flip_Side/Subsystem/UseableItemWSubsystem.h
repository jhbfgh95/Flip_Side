// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "UseableItemWSubsystem.generated.h"

class AUseableItemActor;

UCLASS()
class FLIP_SIDE_API UUseableItemWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSelectItem> ItemSlotArray;

	//ItemSlotArray에서 Item ID걍 꺼내와서 비헤이비어들 일단 인스턴스화시키기. instacneUseItem이랑 함께하면 될듯
	//TArray<TSubclassOf<class UUseItemAction>> UseItemActionArray;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	void InitUseitemSlot();

	void InstanceUseItems();

public:
	void SelectWantUseItem(AUseableItemActor* TargetItem);
};
