// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoinDataTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "CoinManagementWSubsystem.generated.h"

class ACoinActor;
class UGridManagerSubsystem;
class UCoinActionManagementWSubsystem;
class UDataManagerSubsystem;
struct FFaceData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRangeWanted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoinAddedToReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleCoinDataChanged);
DECLARE_DELEGATE(FOnAllCoinDead);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleTutorialCoinSlotClicked, ACoinActor*);

UCLASS()
class FLIP_SIDE_API UCoinManagementWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	/** 상점에서 넘긴 슬롯 데이터를 UI용 코인 슬롯 상태로 초기화합니다. */
	void InitializeCoinSlots(const TArray<FCoinTypeStructure>& InCoinSlots);

	bool TryAddReadyCoinFromSlot(int32 SlotNumber);
	bool TryCancelReadyCoin(int32 CoinInstanceID);
	bool ReplaceReadyCoinWithSample(int32 FrontWeaponID, int32 BackWeaponID, int32 ReadySlotNumber);

	const TArray<FBattleCoinSlotData>& GetCoinSlots() const { return CoinSlots; }
	const TArray<FReadyCoinData>& GetReadyCoinData() const { return ReadyCoins; }

	void SetCoinReadyPhase(bool bEnabled) { bIsCoinReadyPhase = bEnabled; }
	int32 InstantiateReadyCoinActors();
	ACoinActor* GetRuntimeCoinAtReadySlot(int32 ReadySlotIndex) const;

	// CoinBehaviorPhase에서 Actor가 생성된 뒤 사망 처리와 함께 사용할 예정입니다.
	bool RemoveReadyCoinByInstanceID(int32 CoinInstanceID);

	void InitBattleReadyCoin();
	void CheckBattleReadyCoinAlive();
	TArray<ACoinActor*> GetReadyCoins() const;
	void SetBattleCoinItemFlags(bool bEnabled);
	bool IsCoinInBattleReady(ACoinActor* InCoin) const;
	bool IsCoinIdInBattleReady(int32 TargetID) const;
	void LockCoinReady(ACoinActor* TargetCoin);
	int32 CalculateCoinPrice() const;
	int32 CalculateCoinCount() const;

	UPROPERTY(BlueprintAssignable, Category = "Events|Coin")
	FOnBattleCoinDataChanged OnBattleCoinDataChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events|Hover")
	FOnRangeWanted OnRangeWanted;

	UPROPERTY(BlueprintAssignable, Category = "Events|Sound")
	FOnCoinAddedToReady OnCoinAddedToReady;

	FOnAllCoinDead OnAllCoinDead;
	FOnBattleTutorialCoinSlotClicked OnBattleTutorialCoinSlotClicked;

private:
	static constexpr int32 MaxReadyCoinCount = 10;

	/** 실제 보유 코인이 없을 때 DB 무기 ID만 지정한 10칸의 테스트 슬롯을 구성합니다. */
	void CreateDummyCoinSlots();
	FBattleCoinSlotData* FindCoinSlot(int32 SlotNumber);
	const FBattleCoinSlotData* FindCoinSlot(int32 SlotNumber) const;
	int32 FindEmptyReadyCoinSlotIndex() const;
	int32 AllocateCoinInstanceID() const;
	int32 GetReadyCoinCount() const;
	bool ResolveCoinSlotHP(
		const FCoinTypeStructure& CoinSlot,
		const UDataManagerSubsystem* DataManager,
		int32& OutHP) const;
	ACoinActor* SpawnRuntimeCoinActor(
		int32 ReadySlotIndex,
		const FTransform& SpawnTransform,
		bool bHiddenInGame,
		bool bCollisionEnabled);
	void BroadcastCoinDataChanged();
	void BindRuntimeCoinInteraction(ACoinActor* RuntimeCoin);
	void UnbindRuntimeCoinInteraction(ACoinActor* RuntimeCoin);
	void HandleRuntimeCoinDeathStarted(ACoinActor* DeadCoin);
	void ReleaseAndDestroyRuntimeCoin(int32 ReadySlotIndex);

	UPROPERTY()
	TArray<FBattleCoinSlotData> CoinSlots;

	UPROPERTY()
	TArray<FReadyCoinData> ReadyCoins;

	/** ReadyCoin UI의 0~9번 칸과 같은 인덱스를 사용하며, 빈 칸을 압축하지 않습니다. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<ACoinActor>> RuntimeCoinsByReadySlot;

	UPROPERTY(Transient)
	TObjectPtr<UGridManagerSubsystem> GridManager = nullptr;

	// 생성된 필드 코인의 호버·클릭 입력을 전투 행동 파이프라인에 한 번만 연결합니다.
	UPROPERTY(Transient)
	TObjectPtr<UCoinActionManagementWSubsystem> CoinActionManager = nullptr;

	// BattleManager가 첫 CoinReadyPhase를 알리기 전에도 Ready UI 입력을 받을 수 있게 둡니다.
	bool bIsCoinReadyPhase = true;
};
