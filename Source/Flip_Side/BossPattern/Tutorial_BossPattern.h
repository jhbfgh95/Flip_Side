// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossPattern/BossPatternBase.h"
#include "Tutorial_BossPattern.generated.h"

/**
 * 튜토리얼 보스의 공격 패턴 3종 순서대로
	1. Row 공격 
	2. Column 공격
	3. 쉴드 힐 
 */
UCLASS()
class FLIP_SIDE_API UTutorial_BossPattern : public UBossPatternBase
{
	GENERATED_BODY()
public:
	virtual void ExecutePattern(
		UBossManagerSubsystem* BossManager,
		ABossActor* Boss,
		const TArray<FGridPoint>& InLockedCells,
		const TArray<ACoinActor*>& InLockedTargets, int32 PatternNum) override;	
};
