// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "Subsystems/Subsystem.h"
#include "CoinDataTypes.h"
#include "CoinSlotActor.h"
#include "CoinManagementWSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRangeWanted);

class ACoinActor;

UCLASS()
class FLIP_SIDE_API UCoinManagementWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FCoinTypeStructure> CoinSlotDataArray;

	UPROPERTY()
	TArray<ACoinActor*> BattleReadyCoins;

	UPROPERTY()
	TArray<ACoinActor*> LiveCoinStacks;

	UPROPERTY()
	TArray<ACoinSlotActor*> CoinSlots;

	UPROPERTY()
    class UW_ReadyAndSlotCoinInfo* ReadyCoinInfoWidgetInstance = nullptr;

	UPROPERTY()
	class UCoinActionManagementWSubsystem* CoinActionManager = nullptr;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	void InitCoinSlot();

	void InstanceCoins();

	void BindCoinEvents(ACoinActor* CoinActor);

	void SetCoinInfoWidgetData(
		FCoinWidgetInfoData& FrontWeaponData,
		FCoinWidgetInfoData& BackWeaponData
	);

	void ArrangeSlotCoins(int32 FrontWeaponID);

/* 델리게이트 바인딩 함수들 */
protected:
	UFUNCTION()
    void HandleReadyCoinHovered(ACoinActor* HoveredCoin);

	UFUNCTION()
	void HandleCoinSlotHovered(class ACoinSlotActor* TargetCoinSlot);

	UFUNCTION()
	void HandleCoinUnHovered();
	
	UFUNCTION()
	void HandleCoinSlotUnHovered();
	UFUNCTION()
	void HandleReadyCoinClicked(ACoinActor* ClickedCoin);

	UFUNCTION()
	void HandleCoinSlotClicked(ACoinActor* ReadyTargetCoin);

public:
	//서랍 초기화
	void InitBattleReadyCoin();

	void CheckBattleReadyCoinAlive();

	void AddBattleReadyCoins(ACoinActor* SelectCoinActor, bool bArrangeSlot = true);

	// 서랍에 들어간 코인을 다시 클릭 시 취소 로직
    void RemoveBattleReadyCoins(ACoinActor* SelectCoinActor);

	TArray<ACoinActor*> GetReadyCoins() const;

	bool IsCoinInBattleReady(ACoinActor* InCoin) const;

	bool IsCoinIdInBattleReady(int32 TargetID) const;

	void LockCoinReady(ACoinActor* TargetCoin);

	//외부 사거리 카메라에 바인딩할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Events|Hover")
	FOnRangeWanted OnRangeWanted;
};
