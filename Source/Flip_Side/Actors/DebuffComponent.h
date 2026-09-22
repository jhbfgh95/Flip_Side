#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataTypes/CoinStatDataTypes.h"
#include "DebuffComponent.generated.h"

// 상태 데이터 변경과 실제 스탯/행동 변경을 구분하여 UI만 필요한 갱신도 전달합니다.
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDebuffChanged, const FStatusEffectInstance&, bool);
DECLARE_DELEGATE_RetVal_OneParam(bool, FConsumeIncomingCC, const FStatusEffectInstance&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDebuffCCChanged, ECCTypes, CCType);

namespace DebuffTypeID
{
	constexpr int32 Blind = -1001;
	constexpr int32 Stun = -1002;
}

/** 코인/보스 공통 디버프 저장소. 독, DB, UI, 기본 스탯은 소유하지 않습니다. */
UCLASS(ClassGroup=(Status), meta=(BlueprintSpawnableComponent))
class FLIP_SIDE_API UDebuffComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UDebuffComponent();
	UFUNCTION(BlueprintCallable, Category="Status|Debuff")
	bool ApplyDebuff(FStatusEffectInstance Effect);
	UFUNCTION(BlueprintCallable, Category="Status|Debuff")
	bool ApplyCC(ECCTypes Type, int32 Turns, EStatusEffectSourceType SourceType = EStatusEffectSourceType::None, int32 SourceDataID = -1);
	UFUNCTION(BlueprintPure, Category="Status|Debuff")
	ECCTypes GetCCType() const;
	UFUNCTION(BlueprintPure, Category="Status|Debuff")
	bool HasAnyDebuff() const { return !Effects.IsEmpty(); }
	UFUNCTION(BlueprintPure, Category="Status|Debuff")
	TArray<FStatusEffectInstance> GetDebuffs() const { return Effects; }
	UFUNCTION(BlueprintCallable, Category="Status|Debuff")
	void ClearDebuffs();
	UFUNCTION(BlueprintCallable, Category="Status|Debuff")
	void ClearCC();
	void DecreaseCCDuration(int32 Amount);
	bool RemoveBySerial(int32 Serial);
	int32 RemoveByTypeAndSource(int32 TypeID, EStatusEffectSourceType Source, int32 SourceID);
	void AdvanceTurnEnd(int32 CompletedTurn);
	void Restore(const TArray<FStatusEffectInstance>& SavedEffects);
	void DisableForDeath() { bAcceptsEffects = false; ClearDebuffs(); }
	int32 GetAttackModifier() const;
	int32 GetBattleTurn() const;
	bool bAttackOnly = false; // BossActor가 설정: 보스의 수치 디버프는 공격력만 허용합니다.
	FOnDebuffChanged OnDebuffChanged;
	FConsumeIncomingCC ConsumeIncomingCC; // 그로기 보스가 CC 등록 전에 수치로 전환합니다.
	UPROPERTY(BlueprintAssignable, Category="Status|Debuff")
	FOnDebuffCCChanged OnCCChanged;
private:
	bool IsSupportedEffect(const FStatusEffectInstance& Effect) const;
	bool bAcceptsEffects = true;
	void Notify(const FStatusEffectInstance& Effect, bool bGameplayChanged, ECCTypes PreviousCC);
	UPROPERTY(Transient)
	TArray<FStatusEffectInstance> Effects;
	int32 NextSerial = -2; // 버프의 양수 시리얼과 충돌하지 않습니다.
};
