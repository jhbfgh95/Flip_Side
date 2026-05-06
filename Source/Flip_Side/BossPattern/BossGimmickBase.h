#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BossDataTypes.h"
#include "GridTypes.h"
#include "BossTurnContext.h"
#include "BossGimmickBase.generated.h"

class ABossActor;
class ACoinActor;
class ABase_OtherActor;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class FLIP_SIDE_API UBossGimmickBase : public UObject
{
	GENERATED_BODY()

public:
	FBossGimmickData GimmickData;

	// 배틀 시작 시 1회 호출
	virtual void OnBattleStart(ABossActor* Boss) {}

	// 패턴이 선택된 직후 (텔레그래프 전)
	virtual void OnPatternSelected(ABossActor* Boss, FBossTurnContext& Context) {}

	// 텔레그래프 셀 목록이 결정된 직후 (셀 수정 가능)
	virtual void OnTelegraphBuild(ABossActor* Boss, FBossTurnContext& Context) {}

	// 텔레그래프가 화면에 표시된 직후 (연출 수정 가능)
	virtual void OnTelegraphShown(ABossActor* Boss, FBossTurnContext& Context) {}

	// 패턴 실행 직전 (데미지/스킵 등 Context 수정)
	virtual void OnBeforePatternExecute(ABossActor* Boss, FBossTurnContext& Context) {}

	// 데미지 계산 시 (FinalDamage 수정)
	virtual void OnDamageCalculate(ABossActor* Boss, int32& InOutDamage) {}

	// 패턴 실행 후 (늪 설치 등 부가 효과)
	virtual void OnPatternExecute(ABossActor* Boss, const TArray<FGridPoint>& LockedCells, const TArray<ACoinActor*>& LockedTargets, const TArray<ABase_OtherActor*>& LockedOthers) {}

	// 보스 턴 종료 시
	virtual void OnTurnEnd(ABossActor* Boss) {}

	// 플레이어 턴 시작/종료 시
	virtual void OnPlayerTurnStart(ABossActor* Boss) {}
	virtual void OnPlayerTurnEnd(ABossActor* Boss) {}

	// 코인이 그리드에 착지했을 때
	virtual void OnCoinLanded(ABossActor* Boss, FBossTurnContext& Context) {}
};