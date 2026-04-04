/*
	머테리얼로 색 전체 통제 예정
	- 평소엔 Fresnel을 이용한 흰 색
	- 보스가 공격할 곳은 빨간 색
	
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleClickInterface.h"
#include "BattleHoverInterface.h"
#include "CoinActor.h"
#include "FlipSide_Enum.h"
#include "GridTypes.h"
#include "GridActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGridClicked, AGridActor*, TargetGrid );

USTRUCT(BlueprintType)
struct FCachedColorSet{
	GENERATED_BODY()

	FLinearColor Color = FLinearColor::White;
	float Intensity = 0.f;
	float DoorOpen = 0.f;
};

UCLASS()
class FLIP_SIDE_API AGridActor : public AActor, public IBattleClickInterface, public IBattleHoverInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Grid | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* GridRootComp;

	UPROPERTY(EditAnywhere, Category = "Grid | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* GridMesh;

	UPROPERTY(VisibleAnywhere, Category = "Grid")
	class AActor* CurrentObject;

	//0이 기본, 1이 코인, 2가 아이템
	UPROPERTY(EditAnywhere, Category = "Grid | Component", meta = (AllowPrivateAccess = "true"))
	TArray<FLinearColor> HoverColor;

	FCachedColorSet BossColorset;

protected:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CachedMID = nullptr;

	UMaterialInstanceDynamic* EnsureMID(int32 MaterialIndex = 0);

	bool bIsOccupied = false;

	UPROPERTY(VisibleAnywhere, Category = "Grid | GridXY")
	EGridOccupyingType CurrentOccupying = EGridOccupyingType::None;

	UPROPERTY()
	FVector GridWorldXY;

	UPROPERTY(VisibleAnywhere, Category = "GridXY")
	FGridPoint GridXY;

	bool bBossColorFirstSetted = false;
public:	
	AGridActor();

	FOnGridClicked OnGridClicked;

	int32 HoverFlag = 0;

	virtual void OnClicked_Implementation() override;

	virtual void OnHover_Implementation() override;

	virtual void OnUnhover_Implementation() override;
	
/* Setter */
	UFUNCTION()
	void SetGridXY(int32 GridX, int32 GridY);

	UFUNCTION()
	void SetOccupied(bool IsOccupied, EGridOccupyingType OccupyType, AActor* OccupieActor);
	/*
	//아이템 타입(설치형인지, 아닌지를 좀 알 필요가 있을지도 모르겠음 이건 소모품쪽에서 한 번 이야기 해봐야함)
	UFUNCTION()
	void SetItem(FItemType ItemType)
	*/
/* Getter */
	UFUNCTION()
	bool GetIsOccupied();

	UFUNCTION()
	EGridOccupyingType GetCurrentOccupyingThing();

	UFUNCTION()
	FVector2D GetGridWorldXY();

	//나중에 다운캐스트
	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentOccupied() const;
	
	UFUNCTION()
	FGridPoint GetGridPoint() const;

/* Initialization*/
	UFUNCTION(BlueprintCallable)
	void ClearOccupied();

	UFUNCTION(BlueprintCallable)
	void InitColor();

	bool bIsBossAttack = false;

	UPROPERTY(VisibleAnywhere, Category = "Grid | Preview")
    bool bIsCoinRangePreview = false;
	FCachedColorSet CoinRangeSet;

	UFUNCTION(BlueprintCallable)
	void ApplyCellMaterialParams(const FLinearColor& OutlineColor, float FillIntensity, float DoorOpen);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	
};
