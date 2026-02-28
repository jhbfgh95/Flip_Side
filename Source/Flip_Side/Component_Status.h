// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDataTypes.h"
#include "CoinDataTypes.h"
#include "GridTypes.h"
#include "Component_Status.generated.h"

/*
	- 코인에 어떤 버프가 들어갔는지만 알면 됨
	- 보스도 이거 쓸거임 분화시켜서
	- HP는 걍 Set으로 일단 했음
	- 디버프도 여기 들어올 것임
*/

DECLARE_DELEGATE(FOnCCRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIP_SIDE_API UComponent_Status : public UActorComponent
{
	GENERATED_BODY()

	int32 HP = 1;

	int32 TotalModifiedBehaviorPoint = 0;

	int32 TotalModifiedAttackPoint = 0;

	FGridPoint TotalModifiedRange = 0;

	//걍 군중제어기는 가장 마지막에 들어오는게 적용되는 식으로.
	FCCStructure AppliedCC;

	//디버프도 걍 여기에 쌓아버리기.
	TArray<FBuffStructure> BuffArray;

	bool bIsOnCC = false;
public:	
	UComponent_Status();

	int32 GetHP() const;

	FActionTask GetModifiedStats(int32 WeaponID);

	void SetHP(const int32 ApplyHP);

	void SetStatModifier(FBuffStructure& ModifiedStats);

	void ApplyCC(FCCStructure CC);

	//이건 선택 못하게 하는거
	bool GetOnIsOnCC() const { return bIsOnCC; }

	//개귀찮으니까 걍 배틀매니저에서 한 플로우 지나갈 때마다 -1할꺼임
	int32 CCDuration = 0;

	//클린즈 물약 같은거 걍 이거 불러버리면 돼
	FOnCCRemoved OnCCRemove;

	FOnDead OnDead;
protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CalculateFinalStats();

	void RemoveCC();
};
