#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/AttackAreaTypes.h"
#include "FlipSide_Enum.h"
#include "GridTypes.h"
#include "CoinActionManagementWSubsystem.generated.h"


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

	UPROPERTY()
    class UW_BattleCoinInfo* BattleCoinInfoWidgetInstance = nullptr;

public:
	//이거로 코인 선택하는거 잠궜습니다.
	void SetTurn(const bool bIsTurn) { bIsCorrectTurn = bIsTurn; }

	UFUNCTION()
	void SetSelectedWeapon(class ACoinActor* HoveredCoin);

	UFUNCTION()
	void ExecuteSelectedWeapon(ACoinActor* ClickedCoin);

	void SetCasterCoin(class ACoinActor* CasterCoin);

	void CancelSelectWeapon();

	UFUNCTION()
	void HandleCoinUnHovered();
public:
	EActionInputState CurrentInputState = EActionInputState::None;

	//코인 이펙트 시간
	float CoinNaiagaraTime = 0.5f;

protected:
	//GridManager에서 Range 넣어서 뭐있는지 알아서 UWeaponAction에 넣어줌.
	bool ApplyRangedThings(const FGridPoint& TargetGridPoint);

	void InitWeaponAction();

	void ExecuteNowAction();

	void ExecuteTimeAction(ACoinActor* TargetCoin);

	UFUNCTION()
	void ExecuteGridAction(AGridActor* targetGrid);

	void SetCoinActorForGrid();

	void SetBattleCoinInfo(
		UTexture2D* Icon, const FText& WeaponName, const FText& RawDescription, 
		int32 DefaultBP, int32 ModifiedBP, 
		int32 DefaultAP, int32 ModifiedAP, FLinearColor WeaponColor
	);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
};
