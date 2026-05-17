#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossDataTypes.h"
#include "CoinDataTypes.h"
#include "BossPatternBase.h"
#include "BossGimmickBase.h"
#include "BossActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossAttackEndedDelegate);
DECLARE_DYNAMIC_DELEGATE(FOnDeadDeathMontageEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDeathStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDead);

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

	virtual bool ConsumeCCForBossTurn();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USceneComponent* BossRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Shield")
	class UNiagaraComponent* ShieldEffectComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Boss", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> BossHPWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	class UW_BossHP* BossHpWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class UStaticMeshComponent* FrontBackground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class UStaticMeshComponent* BottomBackground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class UStaticMeshComponent* LeftBackground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class UStaticMeshComponent* RightBackground;

	bool bHasCachedPatternInfo = false;
	int32 CachedPatternIndex = INDEX_NONE;
	FBossPatternBattleData CachedPatternData;

/* Animations */
protected:
	TObjectPtr<class UAnimInstance> AnimInstance = nullptr;

	TObjectPtr<class UAnimMontage> SelectedPatternAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TObjectPtr<class UAnimMontage> BossClearAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TObjectPtr<class UAnimMontage> BossHitAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Death")
	float BossDeadEffectDelay = 1.0f;

	bool bIsDying = false;

	bool bBossDeathFinished = false;

	FTimerHandle BossDeadEffectTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void BossMontageEnded(class UAnimMontage* TargetMontage, bool bInterrupted);

	UFUNCTION()
	void BroadcastBossDeadAfterEffect();

	void UpdateShieldEffect();
	void ApplyCachedPatternInfoToWidget();

/* Can Custom */
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USkeletalMeshComponent* BossMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USceneComponent* BossSelfEffectLoc;

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
	int32 GetAttackPoint() const { return AttackPoint; }

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

	void SetCurrentHP(int32 NewHP) { CurrentHP = NewHP; }
	void SetMaxHP(int32 NewMaxHP);

	void SetCurrentShield(int32 NewShield) { CurrentShield = NewShield; }
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
	bool GetOnIsOnCC() const { return bIsOnCC; }

	UFUNCTION(BlueprintCallable, Category = "Boss|CC")
	ECCTypes GetAppliedCCType() const { return AppliedCC.CCType; }

	UFUNCTION(BlueprintCallable, Category = "Boss|CC")
	bool IsStunned() const { return bIsOnCC && AppliedCC.CCType == ECCTypes::Stun; }

	UFUNCTION()
	void SetTextureOfBackgrounds(
		UTexture2D* Front,
		UTexture2D* Bottom,
		UTexture2D* Left,
		UTexture2D* Right
	);
	
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

	void FinishBossClearAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
	void BossDeadEffect();
};
