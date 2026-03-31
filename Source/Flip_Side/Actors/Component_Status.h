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
	- 보스도 이거 쓸거임 분화시켜서 사실 안될듯? 따로 분리해야할지도
	- HP는 걍 Set으로 일단 했음
	- 디버프도 여기 들어올 것임
*/

#define MAX_SHIELD 15

DECLARE_DELEGATE(FOnCCRemoved);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShieldChanged, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);
//데미지 가공용 델리게이트 -> 회피 등의 그것들 ( CurrentDmg, ModifiedDmg, bIsModified )순서 
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPreTakeDamage, int32, int32&, bool&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCalculateStats, FActionTask&); //스탯용 델리게이트
//공격 전에 들어가는 델리게이트(보호막 융해) / Target, 원본 데미지, 수정할 데미지
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPreGiveDamage, AActor*, int32, int32&); 
//공격 후에 적용되는 델리게이트 (피흡) Target, 실제로 들어간 최종 데미지
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPostGiveDamage, AActor*, int32); 
// UI 갱신용 true가 추가, false가 사라짐
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffListChanged, bool, bIsBuffOn); 

USTRUCT(BlueprintType)
struct FBuffInfo
{
    GENERATED_BODY()

	bool bIsDebuff = false;

    UPROPERTY(BlueprintReadOnly)
    FString BuffName;

    UPROPERTY(BlueprintReadOnly)
    UTexture2D* BuffIcon = nullptr;

	FOnCalculateStats::FDelegate StatDelegate = FOnCalculateStats::FDelegate();
    FOnPreTakeDamage::FDelegate DamageDelegate = FOnPreTakeDamage::FDelegate();
    FOnPreGiveDamage::FDelegate PreGiveDelegate = FOnPreGiveDamage::FDelegate();
    FOnPostGiveDamage::FDelegate PostGiveDelegate = FOnPostGiveDamage::FDelegate();

	FDelegateHandle StatHandle;
	FDelegateHandle DamageHandle;
	FDelegateHandle PreGiveHandle;  
    FDelegateHandle PostGiveHandle;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIP_SIDE_API UComponent_Status : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Weapon | Status")
	int32 MaxHP = 1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon | Status")
	int32 HP = 1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon | Status")
	int32 Shield = 0;

	//걍 군중제어기는 가장 마지막에 들어오는게 적용되는 식으로.
	FCCStructure AppliedCC;

	bool bIsOnCC = false;
public:
	FOnHPChanged OnHpChanged;
 
	FOnShieldChanged OnShieldChanged;

	//클린즈 물약 같은거 걍 이거 불러버리면 돼
	FOnCCRemoved OnCCRemove;

	FOnPreTakeDamage OnPreTakeDamage;

	FOnCalculateStats OnCalculateStats;

	FOnPreGiveDamage OnPreGiveDamage;

	FOnPostGiveDamage OnPostGiveDamage;

	FOnDead OnDead;

	//개귀찮으니까 걍 배틀매니저에서 한 플로우 지나갈 때마다 -1할꺼임
	int32 CCDuration = 0;

	//UI용
    UPROPERTY(BlueprintReadOnly, Category = "Status|Buff")
    TArray<FBuffInfo> ActiveBuffs;

    UPROPERTY(BlueprintAssignable, Category = "Status|Event")
    FOnBuffListChanged OnBuffListChanged;
public:	
	UComponent_Status();

	int32 GetHP() const;

	FActionTask GetModifiedStats();

	//초기 HP 세팅 및 진짜 특수한 경우의 함수 왠만하면 Damage, Heal로 HP관리
	void SetHP(const int32 ApplyHP, bool bIsFirst);

/* 스탯 직접 함수 */
	void ApplyDamage(int32 Damage, AActor* DamageCauser);

	void ApplyHeal(int32 Heal, AActor* HealCauser);

	void ApplyShield(int32 AddShield, AActor* ShieldCauser);

/* 버프 함수 */
public:
	//버프 추가 
	void AddBuffs(const FBuffInfo& Info);

	void ClearTurnBasedBuffs();

	//보호막 추가 데미지 등
	void CheckAttackerPreBuff(AActor* Target, int32 InDmg, int32& OutDmg);
	//피흡 등
	void CheckAttackerPostBuff(AActor* Target, int32 DealtDmg);

/* CC함수 */
	void ApplyCC(FCCStructure CC);

	//이건 선택 못하게 하는거
	bool GetOnIsOnCC() const { return bIsOnCC; }

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RemoveCC();

	void HPChanged(int32 Damage);
};
