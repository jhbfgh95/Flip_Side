// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MoneyGISubsystem.generated.h"

/*
	- 돈 장부 Manager
	- 돈이 들어가는지 / 나가는지
	- 어떤 곳에 얼마나 썼는지
*/

//어떤 거 샀는지 기록용 구조체
UENUM(BlueprintType)
enum class EMoneyRecordType : uint8
{
	Coin UMETA(DisplayName = "Coin"),
	Item UMETA(DisplayName = "Item"),
	Card UMETA(DisplayName = "Card"),
	Weapon UMETA(DisplayName = "Weapon"),
	CoinSlot UMETA(DisplayName = "Coin Slot")
};

USTRUCT(BlueprintType)
struct FStageMoneySummary
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentCoinCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentCoin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentItemCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentItem = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentCardCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentCard = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentWeaponCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentWeapon = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentCoinSlotCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpentCoinSlot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleCoinCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleCoin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleItemCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleItem = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleCardCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleCard = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleWeaponCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleWeapon = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleCoinSlotCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SaleCoinSlot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RefundCoinCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RefundCoin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RefundItemCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RefundItem = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RefundCardCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RefundCard = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TotalSpent = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TotalSale = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TotalRefund = 0;
};

UCLASS()
class FLIP_SIDE_API UMoneyGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	int32 CurrentMoney = 0;
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

/* 상점 */
public:
	UFUNCTION()
	void InitMoney();

	//구매 시 호출
	UFUNCTION()
	bool SpendMoney(EMoneyRecordType RecordType, int32 Cost, int32 Count = 1);

	//판매 시 호출 RecordType으로 코인, 카드
	UFUNCTION()
	void AddSaleMoney(EMoneyRecordType RecordType, int32 Amount, int32 Count = 1);

	//스테이지 보상처럼 장부 기록 없이 현재 돈만 늘릴 때 호출
	UFUNCTION()
	void AddRewardMoney(int32 Amount);

private:
	bool HasEnoughMoney(int32 Cost) const;

/* 배틀 */
public:
	//스테이지 클리어 청산 시 호출한다. 현재 돈을 늘리고 Refund 기록을 남긴다.
	UFUNCTION()
	void AddStageRefund(EMoneyRecordType RecordType, int32 Amount, int32 Count = 1);

	//다음 상점/스테이지 기록을 새로 시작할 때 이번 스테이지 요약만 초기화한다.
	UFUNCTION()
	void ResetStageMoneySummary();

	//StageEnd UI용. 이번 스테이지의 지출/판매/청산 요약을 반환한다.
	UFUNCTION()
	FStageMoneySummary GetLastStageMoneySummary() const;

	//GameClear UI용. 한 게임 전체의 지출/판매/청산 누적 요약을 반환한다.
	UFUNCTION()
	FStageMoneySummary GetRunMoneySummary() const;

	UFUNCTION()
	int32 GetCurrentMoney() const;

	void UpdateMoneyDisplayWidget();
private:
	void EnsureMoneyDisplayWidget();
	//돈 쓰면 타입 맞게 Summary에 넣어줌
	void AddSpendToSummary(FStageMoneySummary& Summary, EMoneyRecordType RecordType, int32 Amount, int32 Count);
	//판거 타입 맞게 Summary에서 빼줌
	void AddSaleToSummary(FStageMoneySummary& Summary, EMoneyRecordType RecordType, int32 Amount, int32 Count);
	//배틀 청산용이라 Coin/Item/Card만 사용한다. -> 청산용 Summary
	void AddRefundToSummary(FStageMoneySummary& Summary, EMoneyRecordType RecordType, int32 Amount, int32 Count);
	//Summary 재계산
	void RecalculateSummaryTotal(FStageMoneySummary& Summary);
	//현재 돈을 늘리는 내부 함수. 판매/청산 기록은 남기지 않는다.
	void AddMoney(int32 Amount);

private:
	FStageMoneySummary LastStageMoneySummary;
	FStageMoneySummary RunMoneySummary;

	UPROPERTY()
	class UW_MoneyDisplay* MoneyDisplayWidgetInstance = nullptr;
};
