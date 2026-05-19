// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridTypes.h"
#include "Subsystems/Subsystem.h"
#include "CoinDataTypes.h"
#include "CoinSlotActor.h"
#include "CoinManagementWSubsystem.generated.h"

class ACoinActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRangeWanted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoinAddedToReady);
DECLARE_DELEGATE(FOnAllCoinDead);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleTutorialCoinSlotClicked, ACoinActor*);

UCLASS()
class FLIP_SIDE_API UCoinManagementWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FCoinTypeStructure> CoinSlotDataArray;

	//서랍으로 가는 코인
	UPROPERTY()
	TArray<ACoinActor*> BattleReadyCoins;

	//그리드에서 살아남은 코인
	UPROPERTY()
	TArray<ACoinActor*> LiveCoinStacks;

	//게임 오버용 전체 코인 확인
	UPROPERTY()
	TArray<ACoinActor*> GameOverCheckArray;

	UPROPERTY()
	TArray<ACoinSlotActor*> CoinSlots;

	UPROPERTY()
    class UW_ReadyAndSlotCoinInfo* ReadyCoinInfoWidgetInstance = nullptr;

	UPROPERTY()
	class UCoinActionManagementWSubsystem* CoinActionManager = nullptr;

	bool bIsCoinReadyTurn = false;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	void InitCoinSlot();

	void AddEmptyFallbackCoinSlot();

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

	UFUNCTION()
	void HandleCoinDestroyed(AActor* DestroyedCoin);

	UFUNCTION()
	void OnArrangeSlotMoveComplete() {}
public:
	//서랍 초기화
	void InitBattleReadyCoin();

	void CheckBattleReadyCoinAlive();

	void AddBattleReadyCoins(ACoinActor* SelectCoinActor, bool bArrangeSlot = true);

	// 서랍에 들어간 코인을 다시 클릭 시 취소 로직
    void RemoveBattleReadyCoins(ACoinActor* SelectCoinActor);

	TArray<ACoinActor*> GetReadyCoins() const;

	void SetBattleCoinItemFlags(bool bEnabled);

	bool IsCoinInBattleReady(ACoinActor* InCoin) const;

	bool IsCoinIdInBattleReady(int32 TargetID) const;

	void LockCoinReady(ACoinActor* TargetCoin);

	void SetCoinReadyTurn(const bool TurnFlag) { bIsCoinReadyTurn = TurnFlag;}

	int32 CalculateCoinPrice() const;

	int32 CalculateCoinCount() const;

	//외부 사거리 카메라에 바인딩할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Events|Hover")
	FOnRangeWanted OnRangeWanted;

	UPROPERTY(BlueprintAssignable, Category = "Events|Sound")
	FOnCoinAddedToReady OnCoinAddedToReady;

	FOnAllCoinDead OnAllCoinDead;

	FOnBattleTutorialCoinSlotClicked OnBattleTutorialCoinSlotClicked;
};
