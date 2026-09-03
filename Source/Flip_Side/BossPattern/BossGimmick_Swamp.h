#pragma once

#include "CoreMinimal.h"
#include "BossGimmickBase.h"
#include "BossGimmick_Swamp.generated.h"

class ACoinActor;

UCLASS()
class FLIP_SIDE_API UBossGimmick_Swamp : public UBossGimmickBase
{
	GENERATED_BODY()

public:
	// 패턴 실행 직전: 고정 데미지(1) 보정, 3번 패턴은 코인별 개별 데미지를 위해 엔진 기본 데미지를 스킵
	virtual void OnBeforePatternExecute(ABossActor* Boss, FBossPhaseContext& Context) override;

	// 패턴 실행 후: 맞은 코인에게 디버프 부여(0/1번 패턴) 또는 디버프 여부에 따른 개별 데미지(2번 패턴)
	virtual void OnPatternExecute(ABossActor* Boss, const TArray<FGridPoint>& LockedCells, const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers) override;

private:
	int32 PendingPatternIndex = -1;
};
