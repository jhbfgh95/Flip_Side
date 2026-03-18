#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossPatternBase.h"
#include "BossDataTypes.h"
#include "BossActor.generated.h"

class UComponent_Status;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossAttackEndedDelegate);

UCLASS()
class FLIP_SIDE_API ABossActor : public AActor
{
	GENERATED_BODY()

public:
	ABossActor();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	class USkeletalMeshComponent* BossMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	int32 ThemeID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	int32 BossID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	FString BossName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	int32 AttackPoint = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	TObjectPtr<UComponent_Status> StatusComp;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Boss|Pattern")
	TObjectPtr<UBossPatternBase> Pattern;

	TObjectPtr<class UAnimMontage> SelectedPatternAnim = nullptr;

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

	UFUNCTION(BlueprintCallable, Category = "Boss")
	UComponent_Status* GetStatusComponent() const { return StatusComp; }

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	int32 GetPatternCount() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	UBossPatternBase* GetPattern() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void SetPatternAnim(class UAnimMontage* TargetMontage);

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void AttackMontageEnded(class UAnimMontage* TargetMontage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void PlayTelegraph();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void PlayAttack();

public:
	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FOnBossAttackEndedDelegate OnBossAttackEnded;
};