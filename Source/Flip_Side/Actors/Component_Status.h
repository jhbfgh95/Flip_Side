// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataTypes/WeaponDataTypes.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/CoinStatDataTypes.h"
#include "DataTypes/GridTypes.h"
#include "Component_Status.generated.h"

#define MAX_SHIELD 15
#define MAX_CCDURATION 999

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCActived);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHPChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShieldChanged, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPreTakeDamage, int32, int32&, bool&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCalculateStats, FActionTask&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPreGiveDamage, AActor*, int32, int32&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPostGiveDamage, AActor*, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffListChanged, bool, bIsBuffOn);

// PlayerController의 호버 캐시와 범위 미리보기가 최신 스탯을 다시 계산할 때 구독합니다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponStatsChanged, const FWeaponStatsChangedEvent&);
// CoinActor VFX와 추후 버프 UI가 상태효과의 추가·제거를 감지할 때 구독합니다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatusEffectsChanged, const FStatusEffectsChangedEvent&);

/** 기존 카드·아이템 로직이 FActionTask 기반으로 동작하는 동안만 유지하는 호환 버프 데이터입니다. */
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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FLIP_SIDE_API UComponent_Status : public UActorComponent
{
	GENERATED_BODY()

public:
	UComponent_Status();

	int32 GetHP() const { return HP; }
	int32 GetMaxHP() const { return MaxHP; }
	int32 GetBaseMaxHP() const { return BaseMaxHP; }
	int32 GetShield() const { return Shield; }
	bool IsDead() const { return bIsDead; }
	int32 GetWeaponStatRevision() const { return WeaponStatRevision; }
	EFaceState GetCurrentFace() const { return CurrentFace; }
	const TArray<FStatusEffectInstance>& GetStatusEffects() const { return ActiveStatusEffects; }

	// CoinManager가 CoinActor를 만들 때 양면 무기 정의와 이전 턴 생존 상태를 한 번에 설정합니다.
	bool InitializeCoinStats(const FCoinStatInitializeData& InitializeData);

	// 호버 UI가 현재 활성 효과까지 합산한 특정 면의 표시값을 요청할 때 사용합니다.
	UFUNCTION(BlueprintPure, Category = "Status|Weapon")
	FResolvedWeaponFaceStats ResolveFaceStats(EFaceState Face) const;

	// ReadyCoin처럼 Actor가 없는 상태에서도 동일한 버프 계산식을 재사용합니다.
	static FResolvedWeaponFaceStats ResolveFaceStatsFromData(
		const FWeaponFaceStats& FaceStats,
		const TArray<FStatusEffectInstance>& StatusEffects
	);

	// 호버 후보를 만들고 클릭 시 리비전을 비교해 UWeapon_Action으로 승격할 때 사용합니다.
	UFUNCTION(BlueprintPure, Category = "Status|Weapon")
	FWeaponActionSnapshot BuildActionSnapshot(EFaceState Face) const;

	// 동일 BuffTypeID도 별도 인스턴스로 보존하며 NonStackable 디버프만 중복을 거부합니다.
	UFUNCTION(BlueprintCallable, Category = "Status|Effect")
	bool AddStatusEffect(FStatusEffectInstance StatusEffect);

	// 개별 버프 출처가 해제될 때 BuffInstanceSerial로 정확한 한 인스턴스만 제거합니다.
	UFUNCTION(BlueprintCallable, Category = "Status|Effect")
	bool RemoveStatusEffectByInstanceSerial(int32 BuffInstanceSerial);

	// 다음 SettingPhase 진입 시 CoinManager가 TurnOnly 효과를 버리고 영속 효과만 저장합니다.
	UFUNCTION(BlueprintCallable, Category = "Status|Effect")
	int32 RemoveTurnOnlyStatusEffects();

	// CoinManager가 Actor 파괴 전에 HP·보호막·전투 지속 효과만 가져갑니다.
	FCoinRuntimeStateSnapshot ExportRuntimeState() const;

	// CoinManager가 다음 CoinBehaviorPhase Actor에 저장된 전투 상태를 복원합니다.
	bool ImportRuntimeState(const FCoinRuntimeStateSnapshot& RuntimeState);

	// Deferred Spawn 뒤 BeginPlay한 CoinActor가 현재 상태 VFX를 한 번 동기화할 때 사용합니다.
	void RefreshStatusEffectEvents() const;

	// 기존 WeaponLogic/Card/Item을 새 스냅숏 경로로 이관할 때까지 유지하는 호환 API입니다.
	FActionTask GetModifiedStats();
	void SetFaceWeaponStats(int32 FrontBP, int32 FrontAP, int32 BackBP, int32 BackAP);
	int32 GetWeaponBP() const { return BP.IsValidIndex(CurrentFaceIndex) ? BP[CurrentFaceIndex] : 0; }
	int32 GetWeaponAP() const { return AP.IsValidIndex(CurrentFaceIndex) ? AP[CurrentFaceIndex] : 0; }

	// 초기화 이외의 호출은 죽은 코인을 되살리지 않으며 현재 HP만 안전하게 변경합니다.
	void SetHP(int32 ApplyHP, bool bIsFirst);
	void ApplyFaceWeaponStat(EFaceState Face);
	void ApplyDamage(int32 Damage, AActor* DamageCauser);
	void ApplyHeal(int32 Heal, AActor* HealCauser);
	void ApplyShield(int32 AddShield, AActor* ShieldCauser);

	// 기존 델리게이트 버프 호출부를 새 상태효과 API로 이관할 때까지 유지합니다.
	void AddBuffs(const FBuffInfo& Info);
	void ClearDebuffs();
	void ClearTurnBasedBuffs();
	void CheckAttackerPreBuff(AActor* Target, int32 InDmg, int32& OutDmg);
	void CheckAttackerPostBuff(AActor* Target, int32 DealtDmg);

	void ApplyCC(FCCStructure CC);
	bool GetOnIsOnCC() const { return bIsOnCC; }
	void DecreaseCCDuration(int32 WantToDecreaseCCDuration);

	FOnHPChanged OnHpChanged;
	FOnMaxHPChanged OnMaxHPChanged;
	FOnShieldChanged OnShieldChanged;
	FOnCCRemoved OnCCRemove;
	FOnPreTakeDamage OnPreTakeDamage;
	FOnCalculateStats OnCalculateStats;
	FOnPreGiveDamage OnPreGiveDamage;
	FOnPostGiveDamage OnPostGiveDamage;
	FOnDead OnDead;
	FOnCCActived OnCCActived;
	FOnWeaponStatsChanged OnWeaponStatsChanged;
	FOnStatusEffectsChanged OnStatusEffectsChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Status|Buff")
	TArray<FBuffInfo> ActiveBuffs;

	UPROPERTY(BlueprintAssignable, Category = "Status|Event")
	FOnBuffListChanged OnBuffListChanged;

protected:
	virtual void BeginPlay() override;

private:
	const FWeaponFaceStats* FindFaceStats(EFaceState Face) const;
	void MarkWeaponStatsDirty(EWeaponStatChangeFlags ChangeFlags);
	void BroadcastStatusEffectChanged(const FStatusEffectInstance& StatusEffect) const;
	int32 CountStatusEffectStacks(const FStatusEffectInstance& StatusEffect) const;
	void RecalculateMaxHPFromEffects(bool bIncreaseCurrentHPForPositiveDelta);
	void RemoveLegacyBuffAt(int32 Index);
	void HandleDeathIfNeeded();
	void RemoveCC();
	void HPChanged(int32 Damage);

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Status")
	int32 BaseMaxHP = 1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Status")
	int32 MaxHP = 1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Status")
	int32 HP = 1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Status")
	int32 Shield = 0;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Status")
	bool bIsDead = false;

	// 코인 런타임 초기화는 한 Actor 수명에 한 번만 허용해 사망 후 재초기화를 통한 부활을 막습니다.
	bool bHasRuntimeInitialized = false;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Definition")
	FWeaponFaceStats FrontWeaponStats;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Definition")
	FWeaponFaceStats BackWeaponStats;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Status")
	EFaceState CurrentFace = EFaceState::None;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Status")
	int32 WeaponStatRevision = 0;

	UPROPERTY(VisibleAnywhere, Category = "Status|Effect")
	TArray<FStatusEffectInstance> ActiveStatusEffects;

	int32 NextBuffInstanceSerial = 1;

	// 앞면 BP[0], 뒷면 BP[1]인 기존 행동 코드 호환 배열입니다.
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Legacy")
	TArray<int32> BP = {0, 0};

	// 앞면 AP[0], 뒷면 AP[1]인 기존 행동 코드 호환 배열입니다.
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Legacy")
	TArray<int32> AP = {0, 0};

	int32 CurrentFaceIndex = 0;
	FCCStructure AppliedCC;
	int32 CCDuration = 0;
	bool bIsOnCC = false;
};
