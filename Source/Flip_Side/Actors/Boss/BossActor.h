#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossDataTypes.h"
#include "CoinDataTypes.h"
#include "BossPatternBase.h"
#include "BossGimmickBase.h"
#include "Actors/DebuffComponent.h"
#include "BossActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossAttackEndedDelegate);
DECLARE_DYNAMIC_DELEGATE(FOnDeadDeathMontageEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDeathStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDead);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBossHUDDataChanged, const FBossHUDData&);

UCLASS()
class FLIP_SIDE_API ABossActor : public AActor
{
	GENERATED_BODY()


protected:
/* Default Status */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss | Data")
	FString BossName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss | Stat")
	int32 AttackPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss | Stat")
	int32 MaxHP = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss | Stat")
	int32 CurrentHP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss | Stat")
	int32 MaxShield = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss | Stat")
	int32 CurrentShield = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss | CC")
	FCCStructure AppliedCC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss | CC")
	bool bIsOnCC = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss | CC")
	int32 CCDuration = 0;

/*Impelments Datas*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	int32 ThemeID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	int32 BossID = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss | Stat")
	float StageMultiplierStat = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss | Stat")
	float StageMultiplierGimmick = 1.0f;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Boss|Pattern")
	TObjectPtr<UBossPatternBase> Pattern;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Gimmick")
	TArray<TObjectPtr<UBossGimmickBase>> GimmickList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Gimmick")
	TObjectPtr<UBossGimmickBase> ActiveGimmick;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Pillars")
	TSubclassOf<class ABossPillarActor> LeftPillarClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Pillars")
	TSubclassOf<class ABossPillarActor> RightPillarClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Pillars")
	int32 PillarDamageBonus = 0;

	bool bPillarTurnStarted = false;
	bool bHPDamagedThisTurn = false;


/* Stat Functions */
public:
	//Damage 함수. 기본적으로 HP만 있기 때문에, HP가 줄어드는 로직만 존재.
	//하지만, 보호막 등의 "먼저" 줄어들어야 하는 스탯이 있다면, 하위 액터에서 상속해서 쓰면 된다.
	UFUNCTION()
	virtual void ApplyDamage(int32 Damage, AActor* DamageCauser);

	virtual int32 ApplyDamageAndReturnHPDamage(int32 Damage, AActor* DamageCauser);

	virtual int32 ApplyShieldOnlyDamage(int32 Damage, AActor* DamageCauser);

	virtual void ApplyShieldHeal(int32 Heal, AActor* HealCauser);

	virtual void ApplyCC(const FCCStructure& CC);

	virtual void RemoveCC();

	virtual bool ConsumeCCForBossPhase();
	// ApplyCC API와 직접 DebuffComponent 적용 모두 이 기믹 훅을 거칩니다.
	virtual bool TryConsumeIncomingCC(const FStatusEffectInstance& Effect);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Boss|Debuff")
	TObjectPtr<UDebuffComponent> DebuffComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Boss|Debuff")
	TObjectPtr<class USceneComponent> CCEffectLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Boss|Debuff")
	TObjectPtr<class UStaticMeshComponent> CCDisplayMesh;
	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> CCDisplayMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Boss|Debuff")
	TObjectPtr<class UAnimMontage> StunMontage;
	UFUNCTION(BlueprintImplementableEvent, Category="Boss|Debuff")
	void OnCCVisualChanged(ECCTypes CCType);
	UFUNCTION()
	void HandleCCVisualChanged(ECCTypes CCType);
	void HandleDebuffChanged(const FStatusEffectInstance& Effect, bool bGameplayChanged);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USceneComponent* BossRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Shield")
	class UNiagaraComponent* ShieldEffectComponent;

	bool bHasCachedPatternInfo = false;
	int32 CachedPatternIndex = INDEX_NONE;
	FBossPatternBattleData CachedPatternData;

/* Animations */
protected:
	TObjectPtr<class UAnimInstance> AnimInstance = nullptr;

	TObjectPtr<class UAnimMontage> SelectedPatternAnim = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pattern")
	TObjectPtr<class UAnimMontage> BossClearAnim = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pattern")
	TObjectPtr<class UAnimMontage> BossHitAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Death")
	float BossDeadEffectDelay = 1.0f;

	// 쓰러진 자세 조각 메시를 보스 메시 기준으로 맞추는 보정값입니다. 보스 메시에서 그 자세를 그대로 떠 왔다면 기본값 그대로 둡니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Death")
	FTransform BossFracturePoseOffset;

	// 조각이 흩어지는 속도(cm/s)입니다. 보스가 커서 작으면 그 자리에 무너져 내리기만 합니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Death", meta = (ClampMin = "0"))
	float BossFractureImpulseStrength = 1500.f;

	// 보스 발판은 평소 충돌이 없어서, 부서지는 순간 켜야 조각이 발판을 뚫고 떨어지지 않습니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Death")
	bool bFractureFloorCollision = true;

	// 쓰러지는 동작이 멈출 때를 기다리지 않고 마지막 공격을 맞는 순간 부서집니다. 조각 메시가 맞을 때의 자세로 만들어진 보스용입니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Death")
	bool bFractureOnDeathStart = false;

	bool bIsDying = false;

	bool bBossDeathFinished = false;

	bool bBossFracturePlayed = false;

	FTimerHandle BossDeadEffectTimerHandle;

	FTimerHandle BossFractureImpulseTimerHandle;

	FTimerHandle BossFracturePrewarmTimerHandle;

	int32 BossFractureImpulseTries = 0;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void BossMontageEnded(class UAnimMontage* TargetMontage, bool bInterrupted);

	UFUNCTION()
	void BroadcastBossDeadAfterEffect();

	// 보스 메시를 숨기고 같은 자리의 조각 메시를 부서뜨립니다.
	void PlayBossFracture();

	// Chaos가 조각을 나눈 뒤에 흩뜨리는 힘을 줍니다. 보스는 틱을 쓰지 않아 타이머로 확인합니다.
	UFUNCTION()
	void ApplyBossFractureImpulse();

	// 미리 그려 둔 조각 메시를 숨기고 보스 메시 자리로 되돌립니다.
	UFUNCTION()
	void FinishBossFracturePrewarm();

	void UpdateShieldEffect();
	void BroadcastBossHUDDataChanged();

	// 맞은 결과에 맞는 소리를 냅니다(실드 파괴 > 보스 피격 > 실드 피격 중 하나). 쓰러짐 소리는 전투 매니저가 냅니다.
	void PlayDamageSound(bool bShieldBroken, bool bHPDamaged) const;

/* Can Custom */
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USkeletalMeshComponent* BossMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Background")
	class UStaticMeshComponent* BossFloorMesh;

	// 쓰러짐 동작이 멈추는 순간 보스 대신 보여 주고 부서뜨리는 조각 메시입니다. Rest Collection이 비어 있으면 지금처럼 멈춘 모습으로 남습니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Death")
	TObjectPtr<class UGeometryCollectionComponent> BossFractureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USceneComponent* BossSelfEffectLoc;

	// HUD 체력바의 둥근 창에 보스 얼굴을 실시간으로 보여주는 초상화 카메라입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Portrait")
	TObjectPtr<class UBossPortraitComponent> PortraitComponent;

	// 체력바 둥근 창에 보내는 보스 문양(boss_def.boss_icon_path)입니다. 보드 위 보스 코인과 같은 그림입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Portrait")
	TObjectPtr<class UTexture2D> BossIcon;

/*Functions*/
public:
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void InitializeFromBossData(const FBossBattleData& InData);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetThemeID() const { return ThemeID; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetBossID() const { return BossID; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	const FString& GetBossName() const { return BossName; }

/* Status Functions*/
	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetAttackPoint() const;

	TSubclassOf<ABossPillarActor> GetLeftPillarClass() const { return LeftPillarClass; }
	TSubclassOf<ABossPillarActor> GetRightPillarClass() const { return RightPillarClass; }

	void BeginPillarTurn();

	UFUNCTION(BlueprintPure, Category = "Boss|Pillars")
	int32 GetPillarDamageBonus() const { return PillarDamageBonus; }

	// 기믹 배율 계산이 끝난 피해에 더해 누적 한 단계가 항상 피해 +1이 되게 합니다.
	UFUNCTION(BlueprintPure, Category = "Boss|Pillars")
	int32 GetDamageWithPillarBonus(int32 Damage) const;

	UFUNCTION(BlueprintCallable, Category = "Boss")
	float GetStageMultiplierStat() const { return StageMultiplierStat; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	float GetStageMultiplierGimmick() const { return StageMultiplierGimmick; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetCurrentShield() const { return CurrentShield; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetMaxShield() const { return MaxShield; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool HasShield() const { return CurrentShield > 0; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetCurrentHP() const { return CurrentHP; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetMaxHP() const { return MaxHP; }

	virtual FBossHUDData GetBossHUDData() const;

	void SetCurrentHP(int32 NewHP);
	void SetMaxHP(int32 NewMaxHP);

	void SetCurrentShield(int32 NewShield);
	void SetPattern(UBossPatternBase* InPattern) { Pattern = InPattern; }
	void AddGimmick(UBossGimmickBase* InGimmick);
	void SetActiveGimmick(UBossGimmickBase* InGimmick) { ActiveGimmick = InGimmick; }

	void InitShield(int32 ShieldValue);

	UFUNCTION(BlueprintCallable, Category = "Boss|Gimmick")
	UBossGimmickBase* GetActiveGimmick() const { return ActiveGimmick; }

	const TArray<TObjectPtr<UBossGimmickBase>>& GetGimmickList() const { return GimmickList; }

	UFUNCTION(BlueprintCallable, Category = "Boss|Gimmick")
	int32 GetGimmickCount() const { return GimmickList.Num(); }


	UFUNCTION(BlueprintCallable, Category = "Boss|CC")
	bool GetOnIsOnCC() const { return GetAppliedCCType() != ECCTypes::None; }

	UFUNCTION(BlueprintCallable, Category = "Boss|CC")
	ECCTypes GetAppliedCCType() const { return IsValid(DebuffComponent) ? DebuffComponent->GetCCType() : ECCTypes::None; }

	UFUNCTION(BlueprintCallable, Category = "Boss|CC")
	bool IsStunned() const { return GetAppliedCCType() == ECCTypes::Stun; }
	UFUNCTION(BlueprintPure, Category="Boss|CC")
	bool IsBlinded() const { return GetAppliedCCType() == ECCTypes::Blind; }

/* Getters */
	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	int32 GetPatternCount() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	FVector GetSelfEffectLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	UBossPatternBase* GetPattern() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	bool GetPatternDataList(TArray<FBossPatternBattleData>& OutPatternDataList) const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	bool GetPatternData(int32 PatternIndex, FBossPatternBattleData& OutPatternData) const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void SetPatternAnim(class UAnimMontage* TargetMontage);

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void SetCurrentPatternInfo(int32 PatternIndex, const FBossPatternBattleData& PatternData);

	// 보스 버프 로직에서 호출하면 부여 당시 패턴 아이콘을 코인 상태에 함께 저장합니다.
	UFUNCTION(BlueprintCallable, Category = "Boss|Buff")
	bool ApplyCurrentPatternBuff(class UComponent_Status* TargetStatus, FStatusEffectInstance Buff);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void PlayTelegraph();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void PlayAttack();

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void FinishBossAttack();

	virtual void PlayHitAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
	void DisPlayOutline();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
	void UnDisPlayOutline();

	// 공격 대상 지정 전용: PP 머테리얼의 Stencil 211 분기를 빨강/두께 2로 연결합니다.
	UFUNCTION(BlueprintCallable, Category = "Boss|Outline")
	void SetTargetOutlineEnabled(bool bEnabled);

public:
	ABossActor();

	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FOnBossAttackEndedDelegate OnBossAttackEnded;

	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FOnBossDeathStarted OnBossDeathStarted;

	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FOnBossDead OnBossDead;

	FOnBossHUDDataChanged OnBossHUDDataChanged;

	void FinishBossClearAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
	void BossDeadEffect();
};
