// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossPattern/BossPatternBase.h"
#include "Tutorial_BossPattern.generated.h"

/**
 * 튜토리얼 보스의 공격 패턴 3종 순서대로
	1. 3,2 기준 십자가 공격
	2. 5,2 기준 사각형 공격
	3. 쉴드 힐 
 */
UCLASS()
class FLIP_SIDE_API UTutorial_BossPattern : public UBossPatternBase
{
	GENERATED_BODY()
	virtual void ExecutePattern(
		ABossActor* Boss,
		const TArray<FGridPoint>& InLockedCells,
		const TArray<ACoinActor*>& InLockedTargets,
		const TArray<ABase_OtherActor*>& InLockedOthers,
		int32 PatternNum) override;	
};
