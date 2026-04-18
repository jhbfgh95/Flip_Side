#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossPatternBase.h"
#include "BossDataTypes.h"
#include "CoinDataTypes.h"
#include "BossActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossAttackEndedDelegate);
DECLARE_DYNAMIC_DELEGATE(FOnDeadDeathMontageEnded);
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

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Boss|Pattern")
	TObjectPtr<UBossPatternBase> Pattern;


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

	UPROPERTY(EditDefaultsOnly, Category = "Boss", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> BossHPWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	class UW_BossHP* BossHpWidget = nullptr;

/* Animations */
protected:
	TObjectPtr<class UAnimInstance> AnimInstance = nullptr;

	TObjectPtr<class UAnimMontage> SelectedPatternAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TObjectPtr<class UAnimMontage> BossClearAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TObjectPtr<class UAnimMontage> BossHitAnim = nullptr;


	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void BossMontageEnded(class UAnimMontage* TargetMontage, bool bInterrupted);

/* Can Custom */
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USkeletalMeshComponent* BossMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USceneComponent* BossSelfEffectLoc;

/*Functions*/
public:
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void InitializeFromBossData(const FBossData& InData);

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
	int32 GetCurrentShield() const { return CurrentShield; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetMaxShield() const { return MaxShield; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool HasShield() const { return CurrentShield > 0; }

	UFUNCTION(BlueprintCallable, Category = "Boss|CC")
	bool GetOnIsOnCC() const { return bIsOnCC; }

	UFUNCTION(BlueprintCallable, Category = "Boss|CC")
	ECCTypes GetAppliedCCType() const { return AppliedCC.CCType; }

	UFUNCTION(BlueprintCallable, Category = "Boss|CC")
	bool IsStunned() const { return bIsOnCC && AppliedCC.CCType == ECCTypes::Stun; }
	
/* Getters */
	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	int32 GetPatternCount() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	FVector GetSelfEffectLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	UBossPatternBase* GetPattern() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void SetPatternAnim(class UAnimMontage* TargetMontage);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void PlayTelegraph();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void PlayAttack();

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void FinishBossAttack();

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
	FOnBossDead OnBossDead;
};
