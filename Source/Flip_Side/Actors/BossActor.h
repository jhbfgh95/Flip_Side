#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossPatternBase.h"
#include "BossDataTypes.h"
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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USkeletalMeshComponent* BossMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class UWidgetComponent* BossHPUI = nullptr;

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
public:
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void InitializeFromBossData(const FBossData& InData);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetThemeID() const { return ThemeID; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetBossID() const { return BossID; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	const FString& GetBossName() const { return BossName; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetAttackPoint() const { return AttackPoint; }

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	int32 GetPatternCount() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	UBossPatternBase* GetPattern() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void SetPatternAnim(class UAnimMontage* TargetMontage);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void PlayTelegraph();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void PlayAttack();

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