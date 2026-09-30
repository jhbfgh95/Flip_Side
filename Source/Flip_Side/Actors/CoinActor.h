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
#include "DataTypes/WeaponDataTypes.h"
#include "CoinActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverReadyCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverBattleCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedReadyCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedBattleCoinDelegate, ACoinActor*, HoveredCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinRightClicked, ACoinActor*, ClickedCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemExcuteCoinDelegate, ACoinActor*, ClickedCoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoverCoinDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCoinDeathStarted, ACoinActor*);

UENUM(BlueprintType)
enum class ECoinOutlineState : uint8
{
	Hidden,
	Neutral,
	Buff,
	Debuff,
	Completed,
	SpecialCC
};
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

	// CoinManager가 생성 시 이미 조회한 정의를 보관해 행동 중 DataManager 재조회를 없앱니다.
	UPROPERTY(VisibleAnywhere, Category = "Coin | WeaponData")
	FFaceData FrontWeaponDefinition;

	UPROPERTY(VisibleAnywhere, Category = "Coin | WeaponData")
	FFaceData BackWeaponDefinition;

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

	// 한 턴의 추가 클릭 기회입니다. 무기 내부 반복 횟수와 독립적입니다.
	int32 RemainingAdditionalActions = 0;
	
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

	// BP의 스타일을 월드 MPC로 전달합니다. CoinOutline1~10의 RGB=색, A=두께입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Coin|Outline")
	TObjectPtr<class UMaterialParameterCollection> OutlineParameterCollection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Coin|Outline")
	FLinearColor NeutralOutlineColor = FLinearColor::White;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Coin|Outline")
	FLinearColor BuffOutlineColor = FLinearColor::Green;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Coin|Outline")
	FLinearColor DebuffOutlineColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Coin|Outline")
	FLinearColor CompletedOutlineColor = FLinearColor(0.3f, 0.3f, 0.3f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Coin|Outline")
	FLinearColor HoverOutlineColor = FLinearColor::Yellow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Coin|Outline")
	TMap<ECCTypes, FLinearColor> CCOutlineColors;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Coin|Outline", meta=(ClampMin="0.0"))
	float BuffOutlineThickness = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Coin|Outline", meta=(ClampMin="0.0"))
	float DebuffOutlineThickness = 4.0f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Coin|Outline")
	ECoinOutlineState OutlineState = ECoinOutlineState::Hidden;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Coin|Outline")
	bool bOutlineHovered = false;
	// 런타임 BP에서 색/두께를 수정했다면 이 함수를 호출해 MPC에 반영합니다.
	UFUNCTION(BlueprintCallable, Category="Coin|Outline")
	void RefreshOutline();

	UPROPERTY(EditAnywhere, Category = "Coin | Component")
	class UComponent_Status* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Coin|Debuff")
	TObjectPtr<class UDebuffComponent> DebuffComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Coin|Debuff")
	TObjectPtr<class USceneComponent> CCEffectLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Coin|Debuff")
	TObjectPtr<class UStaticMeshComponent> CCDisplayMesh;
	// CCDisplayMesh의 0번 머테리얼로부터 코인별 MID를 생성합니다.
	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> CCDisplayMaterial;

	// BP가 공용 위치에 실명/기절 메쉬 또는 VFX를 표시하고 None에서 제거합니다.
	UFUNCTION(BlueprintImplementableEvent, Category="Coin|Debuff")
	void OnCCVisualChanged(ECCTypes CCType);

	UFUNCTION()
	void HandleCCVisualChanged(ECCTypes CCType);

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
	void GrantAdditionalActions(int32 Count) { RemainingAdditionalActions += FMath::Max(0, Count); }
	void ClearAdditionalActions() { RemainingAdditionalActions = 0; }
	bool ConsumeAdditionalAction();
	bool GetCoinIsActed() const;
	// 입력 잠금 bIsActed와 분리합니다. 모든 메인 키워드 처리 완료 시 ActionManager가 알립니다.
	void MarkAllMainKeywordsConsumed();
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Coin|Outline")
	bool bAllMainKeywordsConsumed = false;

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

	void SetWeaponDefinitions(const FFaceData& FrontDefinition, const FFaceData& BackDefinition);
	const FFaceData* GetCurrentWeaponDefinition() const;

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

	// Controller의 슬롯 호버 알림입니다. 윤곽선은 RefreshOutline에서 자동 적용합니다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Coin|Highlight")
	void OnReadySlotHighlightChanged(bool bHighlighted);
	void SetReadySlotHighlighted(bool bHighlighted);

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
	// 등장/뒤집기와 피격은 행동 상태와 독립적으로 유지하여 겹친 연출이 모두 끝나야 복구합니다.
	bool bOutlineJumpActive = false;
	bool bOutlineHitActive = false;
	bool bReadySlotHighlighted = false;
	bool bFieldOutlineHovered = false;
	bool bOutlinePhaseActive = false;
	bool bOutlineWasEligible = false;
	bool bOutlineParameterWarningLogged = false;
	UFUNCTION()
	void HandleOutlinePhaseChanged(EPhaseState Phase);
	void HandleOutlineStatsChanged(const FWeaponStatsChangedEvent& ChangedEvent);
	bool IsOutlineEligible() const;

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
