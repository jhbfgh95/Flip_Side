#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CardTypes.h"
#include "DataTypes/GridTypes.h"
#include "StageCardWSubsystem.generated.h"

class UGridManagerSubsystem;
class UCrossingLevelGISubsystem;
class UDataManagerSubsystem;
class UBattleLevelActingWSubsystem;
class ACoinActor;
struct FCoinOnGridInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStageHandCardSet, int32, HandIndex, FCardData, CardInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStageHandCardCleared, int32, HandIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStageHandCardActive, int32, HandIndex, bool, IsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleCardDataChanged);

struct FStageCardRuntimeState
{
    int32 CardID = INDEX_NONE;
    bool bActive = false;
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers> Applied;
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers> EntryModifiers;
};

UCLASS()
class FLIP_SIDE_API UStageCardWSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()
protected:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
public:
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    UPROPERTY(BlueprintAssignable) FStageHandCardSet OnHandCardSet;
    UPROPERTY(BlueprintAssignable) FStageHandCardCleared OnHandCardCleared;
    UPROPERTY(BlueprintAssignable) FStageHandCardActive OnStageHandCardActive;
    UPROPERTY(BlueprintAssignable, Category = "StageCard") FOnBattleCardDataChanged OnBattleCardDataChanged;

    int32 CardPrice = 0;
    UFUNCTION(BlueprintCallable) void RefreshHandFromGI();
    UFUNCTION(BlueprintCallable) void RemoveHandCard(int32 HandIndex);
    bool ChangeHandCard(int32 CardID, int32 CardSlot);
    UFUNCTION(BlueprintCallable) bool TryGetHandCard(int32 HandIndex, FCardData& Out) const;
    void GetBattleCardSlots(TArray<FBattleCardSlotViewData>& OutCardSlots) const;
    UFUNCTION(BlueprintCallable, Category = "Debug|Stage Card") bool TestCardGenerate();

    // BattleManager가 코인 생성/면/좌표 확정 후 턴당 한 번 호출합니다.
    void BeginCardTurn();
    void StopCardEvaluation() { bTurnInitialized = false; }
    // Tick 및 클릭 직전 동기화. 추가 클릭 지급이나 무작위 칸 선정은 반복하지 않습니다.
    void ExecuteCardsEffect();
    void ClearPromotionHighlight();
    UFUNCTION(BlueprintCallable) FCoinCardModifiers GetModifiersForCoin(ACoinActor* Coin) const;
    UFUNCTION(BlueprintCallable) void ClearAllModifiers();
    void SettingDoSettingPhase();
    int32 GetCardPrice() { return static_cast<int32>(CardPrice / 2); }
    int32 GetCardCount() const;

private:
    static constexpr int32 HandCount = 3;
    static constexpr int32 CardBuffTypeBase = 30000;
    UPROPERTY() TArray<FCardData> HandCards;
    UPROPERTY() TArray<bool> bHasCard;
    FStageCardRuntimeState Runtime[HandCount];
    bool bTurnInitialized = false;
    int32 BattleEntryGold = 0;
    bool bRefreshingEffects = false;

    bool TryLoadCardData(int32 CardID, FCardData& Out) const;
    void ClearSlot(int32 HandIndex, bool bNotify);
    void CollectCoinsOnField(TArray<FCoinOnGridInfo>& OutCoins) const;
    void BuildSourceSlots(TMap<int32, int32>& OutSlots) const;
    void SyncCardModifiers(int32 Slot, const TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers>& Desired);
    void SetCardActive(int32 Slot, bool bActive);
    void UnActiveCardUI();

    UPROPERTY() UGridManagerSubsystem* GridSubsys = nullptr;
    UPROPERTY() UCrossingLevelGISubsystem* CrossingGI = nullptr;
    UPROPERTY() UDataManagerSubsystem* DM = nullptr;
    UPROPERTY() UBattleLevelActingWSubsystem* ActingManager = nullptr;
    FGridPoint PromotionHighlightedGrid = FGridPoint(-1, -1);
};
