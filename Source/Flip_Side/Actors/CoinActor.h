// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleHoverInterface.h"
#include "BattleClickInterface.h"
#include "BattleRightClickInterface.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/GridTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "CoinActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverReadyCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverBattleCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedReadyCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedBattleCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinRightClicked, ACoinActor*, ClickedCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemExcuteCoinDelegate, ACoinActor*, ClickedCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoverCoinDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCoinDeathStarted, ACoinActor*);
UCLASS()
class ACoinActor : public AActor, public IBattleHoverInterface, public IBattleClickInterface, public IBattleRightClickInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Coin | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* CoinRootComp;

	/** 코인 메시 애니메이션과 분리된 공격 사거리 시작 괄호 기준점입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin | Range Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneComponent> AttackRangeBracketAnchor;

	/** BP_CoinActor에서 코인 바로 앞 위치에 맞추고 메시·머테리얼을 지정합니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin | Range Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> AttackRangeBracketMesh;

	//실 함수는 이거 써야함 (캐싱)
	UPROPERTY()
	class UW_CoinHPWidget* HPWidget = nullptr;

	//인스턴스화된 코인들의 각 번호
	UPROPERTY(VisibleAnywhere, Category = "Coin | ID")
	int32 CoinID = 0;

	//무기 타입(탱딜힐)의 아이디
	UPROPERTY(VisibleAnywhere, Category = "Coin | Type")
	int TypeID = 0;

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
	
	//singleCell 일때만 동작
	UPROPERTY(VisibleAnywhere)
	bool bIsActing = false;

	UPROPERTY(VisibleAnywhere)
	bool ItemFlag = false;

	//이거로 Getter, Setter로 앞뒤 판별
	UPROPERTY(VisibleAnywhere, Category = "Coin | Face")
	EFaceState CurrentFace = EFaceState::None;

	//판때기 위에 올라갈 때 어디에 올라갈지 정해줌
	UPROPERTY(VisibleAnywhere, Category = "Coin | Grid")
	FGridPoint CurrentGridPoint;

	//1이 Acted, 2가 CCOn
	UPROPERTY(EditAnywhere, Category = "Coin | Covercolor")
	TArray<FLinearColor> CoverColors;

public:	
	ACoinActor();

	UPROPERTY(EditAnywhere, Category = "Coin | Component")
	class UComponent_Status* StatComponent;

	//이거 퍼블릭으로 빼면 오히려 커스텀 스킨을 적용할 수 있다고 생각한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Coin | Component")
	class UStaticMeshComponent* CoinMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Coin | Component")
	class UStaticMeshComponent* CoinActedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin | Component")
	class UGeometryCollectionComponent* FracturedCoin;

	//해당 위젯도 마찬가지.
	UPROPERTY(VisibleAnywhere, Category = "Coin | Component")
	TObjectPtr<class UWidgetComponent> CoinHPUI;

	// 같은 타입 코인들 중에서 몇 번째 코인인지 나타내는 인덱스
	UPROPERTY(VisibleAnywhere, Category = "Coin | Battle")
    int32 SameTypeIndex = 0;
	int32 GetSameTypeIndex() const;

	int32 GetFrontWeaponID() const;
	void DecrementSameTypeIndex(); // 인덱스 감소를 위한 함수

    // 인덱스 제어 함수들
    void SetSameTypeIndex(int32 NewIndex);
    void IncrementSameTypeIndex();

	int32 GetCoinID() const;
	int32 GetCoinFrontID() const { return FrontWeaponID; }
	int32 GetCoinBackID() const { return BackWeaponID; }
	EWeaponClass GetWeaponType() const { return WeaponType; }

	void SetCoinIsReady(bool IsReady);
	bool GetCoinIsReady() const;

	void SetCoinIsActed(const bool IsActed);
	bool GetCoinIsActed() const;

	void SetCoinOnBattle(const bool IsOnBattle);
	bool GetCoinOnBattle() const { return bIsOnBattle; }

	void SetCoinIsActing(const bool IsActing);
	bool GetCoinIsActing() const { return bIsActing; }

	void SetCoinItemFlag(const bool IsItem ){ ItemFlag = IsItem; }
	bool GetCoinItemFlag() const { return ItemFlag; }

	bool SetCoinValues(
		int CoinId,
		int FrontId, 
		int BackId,
		EWeaponClass WeaponTypes, 
		UTexture2D* FrontTexture, 
		UTexture2D* BackTexture,
		const FCoinStatInitializeData& StatInitializeData
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

	UPROPERTY(BlueprintAssignable, Category = "Events|Click")
	FOnItemExcuteCoinDelegate OnCoinClickForItemExcute;

	UPROPERTY(BlueprintAssignable, Category = "Events|Click")
	FOnCoinRightClicked OnCoinRightClicked;

	FOnCoinDeathStarted OnCoinDeathStarted;

	virtual void OnHover_Implementation() override;

	virtual void OnUnhover_Implementation() override;

	virtual void OnClicked_Implementation() override;

	virtual void OnRightClicked_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Coin | Outline")
	void CoinHoverOutline();

	UFUNCTION(BlueprintImplementableEvent, Category = "Coin | Outline")
	void CoinUnHoverOutline();

	// BP_CoinActor가 BuffTypeID별 버프·디버프 VFX를 켜고 끌 때 구현할 확장 지점입니다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Coin | Status VFX")
	void OnStatusVisualChanged(
		int32 BuffTypeID,
		EStatusEffectSourceType SourceType,
		int32 SourceDataID,
		int32 TotalStackCount,
		bool bIsDebuff,
		bool bIsActive
	);

/* 연출들 */
public:
	bool DoCoinActAtBattleStart(float XLocation, float YLocation, FSimpleDelegate OnLanded = FSimpleDelegate());

	void SetUIVisibility(const bool bUIVisibile);

	/** PlayerController의 필드 코인 호버 미리보기에서만 호출합니다. */
	UFUNCTION(BlueprintCallable, Category = "Coin | Range Preview")
	void SetAttackRangeBracketVisible(bool bVisible);

protected:
	/* 레디 코인 튀어 오름 */
	FTimerHandle JumpTimerHandle;

	FVector DecidedGridLocation;

	FRotator DecidedCoinRotation;

	float AnimStartXRot = 0.0f;

	float JumpElapsedTime = 0.0f;

	FSimpleDelegate PendingLandingDelegate;
	bool bLandingCallbackPending = false;
	bool bDeathStarted = false;

	UPROPERTY(EditAnywhere, Category = "Jump", meta = (AllowPrivateAccess = "true"))
    float JumpDuration = 0.5f; // 점프 지속 시간

    UPROPERTY(EditAnywhere, Category = "Jump", meta = (AllowPrivateAccess = "true"))
    float JumpHeight = 150.0f; // 튀어오를 높이	

	void UpdateJump();
	void CompleteLandingCallback();
	void RefreshCoinMaterial();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CoinDead();

	UFUNCTION()
    void OnCoinHpChanged(int32 DeltaHP);

	UFUNCTION()
	void OnCCApplied();

	UFUNCTION()
	void OnCCRemoved();

	void HandleStatusEffectsChanged(const FStatusEffectsChangedEvent& ChangedEvent);

    void ResetFlash();

	void SetCover(FLinearColor CoverColor, bool bIsShow);
	void RefreshCover();

	FTimerHandle FlashTimerHandle;
};
