#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/AttackAreaTypes.h"
#include "FlipSide_Enum.h"
#include "GridTypes.h"
#include "CoinActionManagementWSubsystem.generated.h"

/* 코인 매니저에서 어짜피 지역변수로 반복문 돌려서 걍 여기서도 만들어도 상관없을듯; */

UCLASS()
class FLIP_SIDE_API UCoinActionManagementWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	bool bIsCorrectTurn = false;

	//캐싱
	FAttackAreaSpec AreaSpec;

	//반복
	int32 RepeatActionCnt = 1;

	//이거로 공격범위 표시 ㄱㄱ
	TArray<struct FGridPoint> OutCells;

	UPROPERTY()
	class UWeapon_Action* SelectedAction;

	UPROPERTY()
	TArray<FGridPoint> ValidTargetGrids;

	//이거로 이펙트 타이머함
	FTimerHandle AutoActionHandler;

	UPROPERTY()
    class UGridManagerSubsystem* GridManager;
public:
	//이거로 코인 선택하는거 잠궜습니다.
	void SetTurn(const bool bIsTurn) { bIsCorrectTurn = bIsTurn; }

	void SetSelectedWeapon(const struct FActionTask& ActionTask,const FGridPoint& CoinGrid);

	//이럴거면 애초에 Coin만 받았어야 함.. -> 리팩토링 꼭~하세요!
	void SetCasterCoin(class ACoinActor* CasterCoin);

	void ExecuteNowAction();

	void ExecuteTimeAction(const FGridPoint& TargetGridPoint);

	void CancelSelectWeapon();
public:
	EActionInputState CurrentInputState = EActionInputState::None;

	//코인 이펙트 시간
	float CoinNaiagaraTime = 0.5f;
protected:
	//GridManager에서 Range 넣어서 뭐있는지 알아서 UWeaponAction에 넣어줌.
	bool ApplyRangedThings(const FGridPoint& TargetGridPoint);

	void InitWeaponAction();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};
