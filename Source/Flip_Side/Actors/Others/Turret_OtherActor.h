// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Others/Base_OtherActor.h"
#include "AttackAreaTypes.h"
#include "GridTypes.h"
#include "BattleHoverInterface.h"
#include "Turret_OtherActor.generated.h"


UCLASS()
class FLIP_SIDE_API ATurret_OtherActor : public ABase_OtherActor, public IBattleHoverInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Others | Stat", meta = (AllowPrivateAccess = "true"))
	FAttackAreaSpec TurretSpec;

	UPROPERTY(VisibleAnywhere, Category = "Others | Grid", meta = (AllowPrivateAccess = "true"))
	FGridPoint TurretSpawnGrid;

	UPROPERTY(VisibleAnywhere, Category = "Others | Stat")
	int32 AttackPoint = 0;

	UPROPERTY(VisibleAnywhere);
	class ABossActor* CachedBoss = nullptr;

	class UGridManagerSubsystem* GridManager;

	bool bIsActed = false;
public:
	/** 자동포탑을 설치한 무기의 공격 사거리를 그대로 주입합니다. */
	void InitializeTurret(FGridPoint TargetGrid, const FAttackAreaSpec& AttackAreaSpec);

	void SetTurretAttackPoint(const int32 attackPoint) { AttackPoint = attackPoint; }

	virtual void OnClicked_Implementation() override;

	virtual void OnHover_Implementation() override;

	virtual void OnUnhover_Implementation() override;
};
