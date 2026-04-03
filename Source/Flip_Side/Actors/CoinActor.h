// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleHoverInterface.h"
#include "BattleClickInterface.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/GridTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "CoinActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverReadyCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverBattleCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedReadyCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedBattleCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoverCoinDelegate);
UCLASS()
class ACoinActor : public AActor, public IBattleHoverInterface, public IBattleClickInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Coin | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* CoinRootComp;

	//실 함수는 이거 써야함 (캐싱)
	UPROPERTY()
	class UW_CoinHPWidget* HPWidget = nullptr;

	//인스턴스화된 코인들의 각 번호
	UPROPERTY(VisibleAnywhere, Category = "Coin | ID")
	int32 CoinID = 0;

	//무기 타입(탱딜힐)의 아이디
	UPROPERTY(VisibleAnywhere, Category = "Coin | Type")
	int TypeID = 0;

	//슬롯 번호
	UPROPERTY(VisibleAnywhere, Category = "Coin | Slot")
	int32 SlotIndex = -1;

	//무기 타입 ENum 위에거나 이거 둘 중 하나 없앨 예정
	UPROPERTY(VisibleAnywhere, Category = "Coin | Type")
	EWeaponClass WeaponType = EWeaponClass::None;

	//앞면 무기 ID
	UPROPERTY(VisibleAnywhere, Category = "Coin | WeaponID")
	int FrontWeaponID = 0;

	//뒷면 무기 ID
	UPROPERTY(VisibleAnywhere, Category = "Coin | WeaponID")
	int BackWeaponID = 0;

	UPROPERTY(VisibleAnywhere)
	UTexture2D* FrontIconTexture;

	UPROPERTY(VisibleAnywhere)
	UTexture2D* BackIconTexture;

	UPROPERTY(VisibleAnywhere)
	FLinearColor TypeColor;


/* Battle상태 변수들 */
protected:
	//랜덤 앞뒤 정해질 때 즉, SetCoinFace할 때 그냥 해당 WeaponID 넣어버림
	int DecidedWeaponID = 0;

	UPROPERTY(VisibleAnywhere)
	bool bIsReady = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsOnBattle = false;
	UPROPERTY(VisibleAnywhere)
	bool bIsActed = false;
	//이거로 Getter, Setter로 앞뒤 판별
	UPROPERTY(VisibleAnywhere, Category = "Coin | Face")
	EFaceState CurrentFace = EFaceState::None;

	//판때기 위에 올라갈 때 어디에 올라갈지 정해줌
	UPROPERTY(VisibleAnywhere, Category = "Coin | Grid")
	FGridPoint CurrentGridPoint;

	// 슬롯 뭉치의 시작점 (0번 자리 좌표) for 코인 취소 시 원위치 복귀
    UPROPERTY(VisibleAnywhere, Category = "Coin | Location")
    FVector OriginSlotLocation;

public:	
	ACoinActor();

	UPROPERTY(EditAnywhere, Category = "Coin | Component")
	class UComponent_Status* StatComponent;

	//이거 퍼블릭으로 빼면 오히려 커스텀 스킨을 적용할 수 있다고 생각한다.
	UPROPERTY(EditAnywhere, Category = "Coin | Component")
	class UStaticMeshComponent* CoinMesh;

	//해당 위젯도 마찬가지.
	UPROPERTY(VisibleAnywhere, Category = "Coin | Component")
	TObjectPtr<class UWidgetComponent> CoinHPUI;

	// 같은 타입 코인들 중에서 몇 번째 코인인지 나타내는 인덱스
	UPROPERTY(VisibleAnywhere, Category = "Coin | Battle")
    int32 SameTypeIndex = 0;
	int32 GetSameTypeIndex() const;

	int32 GetFrontWeaponID() const;
	void DecrementSameTypeIndex(); // 인덱스 감소를 위한 함수

	// 초기 슬롯 위치 저장 및 반환
    void SetOriginSlotLocation(FVector InLoc);
    FVector GetOriginSlotLocation() const;

    // 인덱스 제어 함수들
    void SetSameTypeIndex(int32 NewIndex);
    void IncrementSameTypeIndex();
	int32 GetSlotNum() const { return SlotIndex; }

	int32 GetCoinID() const;
	int32 GetCoinFrontID() const { return FrontWeaponID; }
	int32 GetCoinBackID() const { return BackWeaponID; }

	void SetCoinIsReady(bool IsReady);
	bool GetCoinIsReady() const;

	void SetCoinIsActed(const bool IsActed) { bIsActed = IsActed; }
	bool GetCoinIsActed() const { return bIsActed; }

	void SetCoinOnBattle(const bool IsOnBattle);
	bool GetCoinOnBattle() const { return bIsOnBattle; }

	void SetCoinValues(
		int CoinId,
		int FrontId, 
		int BackId,
		EWeaponClass WeaponTypes, 
		UTexture2D* FrontTexture, 
		UTexture2D* BackTexture,
		FLinearColor DecideColor,
		int32 CoinHP,
		int32 SlotNum
	);

	/* 앞,뒤 결정 */
	int32 GetCoinFaceID() const;

	EFaceState GetCoinDecidedFace() const;

	FGridPoint GetDecidedGrid() const;

	void SetCoinFace(EFaceState DecidedFace);

	/* BattleGrid에 나올 위치 설정 */
	void SetGridPoint(FGridPoint DecidedGridPoint);

	/*UI관련*/
public:
    UPROPERTY(BlueprintAssignable, Category = "Events|Hover")
    FOnHoverReadyCoinDelegate OnHoverReadyCoin;

    UPROPERTY(BlueprintAssignable, Category = "Events|Hover")
    FOnHoverBattleCoinDelegate OnHoverBattleCoin;

	UPROPERTY(BlueprintAssignable, Category = "Events|Hover")
    FOnUnhoverCoinDelegate OnUnhoverCoin;
 
	UPROPERTY(BlueprintAssignable, Category = "Events|Click")
	FOnClickedReadyCoinDelegate OnClickReadyCoin;

	UPROPERTY(BlueprintAssignable, Category = "Events|Click")
	FOnClickedBattleCoinDelegate OnClickBattleCoin;

	virtual void OnHover_Implementation() override;

	virtual void OnUnhover_Implementation() override;

	virtual void OnClicked_Implementation() override;

/* 연출들 */
public:
	void DoCoinActAtBattleStart(float XLocation, float YLocation);

	void DoCoinActAtBattleStartLeverDown();

	void SetUIVisibility(const bool bUIVisibile);

protected:
	/* 레디 코인 튀어 오름 */
	FTimerHandle JumpTimerHandle;

	FVector DecidedGridLocation;

	FRotator DecidedCoinRotation;

	float AnimStartXRot = 0.0f;

	float JumpElapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Jump", meta = (AllowPrivateAccess = "true"))
    float JumpDuration = 0.5f; // 점프 지속 시간

    UPROPERTY(EditAnywhere, Category = "Jump", meta = (AllowPrivateAccess = "true"))
    float JumpHeight = 150.0f; // 튀어오를 높이	

	void UpdateJump();

	/* 레디 코인 서랍 움직임 */
	FTimerHandle LeverMoveTimerHandle;
	float MoveElapsedTime = 0.0f;

	float MoveTime = 0.6f; 

    float StartX = 0.0f;
    float TargetX = 0.0f;

	void UpdateCoinMoveAtBattleStart();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CoinDead();
};
