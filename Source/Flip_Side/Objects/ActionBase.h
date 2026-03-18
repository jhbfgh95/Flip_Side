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

	UPROPERTY(EditAnywhere, Category = "Action", meta = (AllowPrivateAccess = "true"))
	class ABossActor* InRangeBoss;

	/* //장애물, 포탑 등을 Other이라 칭함.
	UPROPERTY(EditAnywhere, Category = "Action", meta = (AllowPrivateAccess = "true"))
	TArray<class AOtherBase> InRangeOthers;
	*/

	FGridPoint FinalActionRange;

	//실제 로직의 ID -> 무기 ID랑 똑같음 보스, 아이템도 동일
	int32 LogicID = -1;

public:
	virtual FGridPoint GetActionRange() const;

	virtual void SetActionRange(FGridPoint& ActionRange);

	virtual void SetInRangeCoins(ACoinActor* TargetCoin);

	virtual void SetLogicID(const int32 ID);

	virtual void SetInRangeBoss(ABossActor* TargetBoss);

	//virtual void SetInRangeOthers(const AOtherActor* TargetOthers);

	virtual TArray<class ACoinActor*> GetInRangeCoins();

	virtual bool GetInRangeBoss(ABossActor*& OutBoss) const;

	void CreateTestRange();

	virtual void ExecuteAction();

};
