// StageCardWSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CardTypes.h"
#include "FlipSide_Enum.h" // EFaceState, EWeaponClass 등
#include "StageCardWSubsystem.generated.h"

class UGridManagerSubsystem;
class UCrossingLevelGISubsystem;
class UDataManagerSubsystem;
class ACoinActor;

struct FCoinOnGridInfo; // GridTypes 쪽에 있을 가능성 높음
struct FFaceData;
struct FGridPoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStageHandCardSet, int32, HandIndex, FCardData, CardInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStageHandCardCleared, int32, HandIndex);

// 코인별 “이번 턴” 카드 효과 결과(Modifier)
USTRUCT(BlueprintType)
struct FCoinCardModifiers
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) int32 ExtraActions = 0;
    UPROPERTY(BlueprintReadOnly) int32 AttackAdd = 0;
    UPROPERTY(BlueprintReadOnly) int32 RangeAdd = 0;
    UPROPERTY(BlueprintReadOnly) int32 BehaviorAdd = 0;
    UPROPERTY(BlueprintReadOnly) bool bLifeSteal = false;
};

UCLASS()
class FLIP_SIDE_API UStageCardWSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

protected:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

public:
    UPROPERTY(BlueprintAssignable)
    FStageHandCardSet OnHandCardSet;

    UPROPERTY(BlueprintAssignable)
    FStageHandCardCleared OnHandCardCleared;

public:
    // ===== [기존 손패 관리 API 유지] =====
    UFUNCTION(BlueprintCallable)
    void RefreshHandFromGI();

    UFUNCTION(BlueprintCallable)
    void RemoveHandCard(int32 HandIndex);

    UFUNCTION(BlueprintCallable)
    bool TryGetHandCard(int32 HandIndex, FCardData& Out) const;

    

public:
    // ===== [추가: 카드 효과 엔진 API] =====
    // BattleManager/턴 시스템이 타이밍 맞춰 호출하는 버전(권장)
    void ExecuteCardsEffect();

    UFUNCTION(BlueprintCallable)
    FCoinCardModifiers GetModifiersForCoin(ACoinActor* Coin) const;

    UFUNCTION(BlueprintCallable)
    void ClearAllModifiers();

private:
    static constexpr int32 HandCount = 3;

    // 손패(슬롯) 데이터
    UPROPERTY()
    TArray<FCardData> HandCards;

    UPROPERTY()
    TArray<bool> bHasCard;

private:
    // ===== [기존 UI 생성 유지] =====
    void EnsureStageHUD(UWorld& InWorld);

    // CardID -> FCardData
    bool TryLoadCardData(int32 CardID, FCardData& Out) const;

    void ClearSlot(int32 HandIndex, bool bNotify);

private:
    UPROPERTY() UGridManagerSubsystem* GridSubsys = nullptr;
    UPROPERTY() UCrossingLevelGISubsystem* CrossingGI = nullptr;
    UPROPERTY() UDataManagerSubsystem* DM = nullptr;

    // 코인별 카드 효과 결과 캐시(턴마다 초기화 권장)
    UPROPERTY()
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers> CoinMods;

private:
    // ===== [추가: 카드 효과 엔진 유틸] =====
    void CollectCoinsOnField(TArray<FCoinOnGridInfo>& OutCoins) const;
    bool TryGetCoinFaceData(ACoinActor* Coin, FFaceData& OutFace) const;
    int32 GetCoinRangeValue(const FFaceData& Face) const;
    bool AreAllFieldCoinsFront(const TArray<FCoinOnGridInfo>& FieldCoins) const;

private:
    // HUD 위젯 클래스(에디터/세팅에서 지정)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> StageHUDClass;

    UPROPERTY()
    UUserWidget* StageHUDInstance = nullptr;
};