// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes/GridTypes.h"
#include "ActionBase.generated.h"


UCLASS(abstract, Blueprintable, BlueprintType)
class FLIP_SIDE_API UActionBase : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Action", meta = (AllowPrivateAccess = "true"))
	TArray<class ACoinActor*> InRangeCoins;

	/* //보스가 나와야함
	UPROPERTY(EditAnywhere, Category = "Action", meta = (AllowPrivateAccess = "true"))
	class ABossActor* InRangeBoss;
	*/

	/* //장애물, 포탑 등을 Other이라 칭함.
	UPROPERTY(EditAnywhere, Category = "Action", meta = (AllowPrivateAccess = "true"))
	TArray<class AOtherBase> InRangeOthers;
	*/

	//보스, 소모품, 코인 셋 모두 공통으로 쓰는 ActionRange(코인은 한 칸이겠죠?)->버프 생각해서 FinalActionRange라고 칭함
	FGridPoint FinalActionRange;

	//실제 로직의 ID
	int32 ActionLogicID = -1;

public:
	virtual FGridPoint GetActionRange() const;

	virtual void SetActionRange(FGridPoint& ActionRange);

	virtual void SetInRangeCoins(ACoinActor* TargetCoin);

	//virtual void SetInRangeBoss(const ABossActor* TargetBoss);

	//virtual void SetInRangeOthers(const AOtherActor* TargetOthers);

	void CreateTestRange();

	virtual void ExecuteAction();

private:
	bool CheckInCoinArray() const;
	//bool CheckInBossArray() const;
	//bool CheckInOthersArray() const;
};
