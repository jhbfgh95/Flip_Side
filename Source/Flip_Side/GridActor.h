/*
	머테리얼로 색 전체 통제 예정
	- 평소엔 Fresnel을 이용한 흰 색
	- 보스가 공격할 곳은 빨간 색
	
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinActor.h"
#include "FlipSide_Enum.h"
#include "GridTypes.h"
#include "GridActor.generated.h"

UCLASS()
class FLIP_SIDE_API AGridActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Grid | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* GridRootComp;

	UPROPERTY(EditAnywhere, Category = "Grid | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* GridMesh;

	//0 보스, 1 아이템
	UPROPERTY(EditAnywhere, Category = "Grid | Colors", meta = (AllowPrivateAccess = "true"))
	TArray<FLinearColor> TypeColors;

	UPROPERTY()
	TWeakObjectPtr<ACoinActor> CurrentCoin;

protected:
	bool bIsOccupied = false;

	//보스가 공격할건지? 보스가 공격하게 되면 머테리얼 변경
	bool bIsGonnaAttack = false;

	EGridOccupyingType CurrentOccupying = EGridOccupyingType::None;

	UPROPERTY()
	FVector GridWorldXY;

	FGridPoint GridXY;

public:	
	AGridActor();

	UFUNCTION()
	void SetGridXY(int32 GridX, int32 GridY);

	UFUNCTION()
	void SetOccupied(bool IsOccupied, EGridOccupyingType OccupyType);

	UFUNCTION()
	void SetBossAttack(bool bWillAttack);

	/*
	//아이템 타입(설치형인지, 아닌지를 좀 알 필요가 있을지도 모르겠음 이건 소모품쪽에서 한 번 이야기 해봐야함)
	UFUNCTION()
	void SetItem(FItemType ItemType)
	*/
	UFUNCTION()
	bool GetIsOccupied();

	UFUNCTION()
	EGridOccupyingType GetCurrentOccupyingThing();

	UFUNCTION()
	FVector GetGridWorldXY();

	UFUNCTION(BlueprintCallable)
	void SetOccupiedCoin(ACoinActor* Coin);

	UFUNCTION(BlueprintCallable)
	void ClearOccupied();

	UFUNCTION(BlueprintCallable)
	ACoinActor* GetCurrentCoin() const;   // GridManager가 이걸로 ID 뽑음


protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
