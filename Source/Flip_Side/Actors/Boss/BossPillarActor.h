#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridTypes.h"
#include "BossPillarActor.generated.h"

class ABossActor;
class UStaticMeshComponent;

/** 보스와 HP 및 보호막을 공유하는 추가 공격 대상입니다. */
UCLASS(Blueprintable)
class FLIP_SIDE_API ABossPillarActor : public AActor
{
	GENERATED_BODY()

public:
	ABossPillarActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Pillar")
	TObjectPtr<UStaticMeshComponent> PillarMesh;

	void InitializeForBoss(ABossActor* InBoss);
	void SetGridPlacement(const TArray<FGridPoint>& InCells, const FVector& WorldLocation);

	UFUNCTION(BlueprintPure, Category = "Boss Pillar")
	ABossActor* GetOwningBoss() const;

	UFUNCTION(BlueprintPure, Category = "Boss Pillar")
	int32 GetFootprintSize() const;

	UFUNCTION(BlueprintPure, Category = "Boss Pillar")
	int32 GetCurrentHP() const;

	UFUNCTION(BlueprintPure, Category = "Boss Pillar")
	int32 GetCurrentShield() const;

	UFUNCTION(BlueprintCallable, Category = "Boss Pillar")
	int32 ApplyBossDamage(int32 Damage, AActor* DamageCauser);

	const TArray<FGridPoint>& GetOccupiedCells() const { return OccupiedCells; }

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Boss Pillar")
	TWeakObjectPtr<ABossActor> OwningBoss;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss Pillar")
	TArray<FGridPoint> OccupiedCells;
};
