// StageCardWSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataTypes/CardTypes.h"
#include "FlipSide_Enum.h"
#include "Subsystem/CardLogicLibrary.h"
#include "StageCardWSubsystem.generated.h"

class UGridManagerSubsystem;
class UCrossingLevelGISubsystem;
class UDataManagerSubsystem;
class UBattleLevelActingWSubsystem;
class ACoinActor;

struct FCoinOnGridInfo; // GridTypes �ʿ� ���� ���ɼ� ����
struct FFaceData;
struct FGridPoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStageHandCardSet, int32, HandIndex, FCardData, CardInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStageHandCardCleared, int32, HandIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStageHandCardActive, int32, HandIndex, bool, IsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleCardDataChanged);

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

    UPROPERTY(BlueprintAssignable)
    FStageHandCardActive OnStageHandCardActive;

    UPROPERTY(BlueprintAssignable, Category = "StageCard")
    FOnBattleCardDataChanged OnBattleCardDataChanged;

    int32 CardPrice = 0;

public:
    // ===== [���� ���� ���� API ����] =====
    UFUNCTION(BlueprintCallable)
    void RefreshHandFromGI();

    UFUNCTION(BlueprintCallable)
    void RemoveHandCard(int32 HandIndex);

    UFUNCTION(BlueprintCallable)
    bool TryGetHandCard(int32 HandIndex, FCardData& Out) const;

    void GetBattleCardSlots(TArray<FBattleCardSlotViewData>& OutCardSlots) const;

    // 실제 장착 카드가 없을 때 HUD/전투 확인용 카드 3종을 런타임 손패에 생성합니다.
    UFUNCTION(BlueprintCallable, Category = "Debug|Stage Card")
    bool TestCardGenerate();

    

public:
    // ===== [�߰�: ī�� ȿ�� ���� API] =====
    // BattleManager/�� �ý����� Ÿ�̹� ���� ȣ���ϴ� ����(����)
    void ExecuteCardsEffect();

    void ClearPromotionHighlight();

    UFUNCTION(BlueprintCallable)
    FCoinCardModifiers GetModifiersForCoin(ACoinActor* Coin) const;

    UFUNCTION(BlueprintCallable)
    void ClearAllModifiers();

    void SettingDoSettingPhase();

    //Park 추가
    int32 GetCardPrice() { return static_cast<int32>(CardPrice / 2); }
    int32 GetCardCount() const;
private:
    static constexpr int32 HandCount = 3;

    // ����(����) ������
    UPROPERTY()
    TArray<FCardData> HandCards;

    UPROPERTY()
    TArray<bool> bHasCard;

private:
    // CardID -> FCardData
    bool TryLoadCardData(int32 CardID, FCardData& Out) const;

    void ClearSlot(int32 HandIndex, bool bNotify);

private:
    UPROPERTY() UGridManagerSubsystem* GridSubsys = nullptr;
    UPROPERTY() UCrossingLevelGISubsystem* CrossingGI = nullptr;
    UPROPERTY() UDataManagerSubsystem* DM = nullptr;
    UPROPERTY() UBattleLevelActingWSubsystem* ActingManager = nullptr;

    // ���κ� ī�� ȿ�� ��� ĳ��(�ϸ��� �ʱ�ȭ ����)
    UPROPERTY()
    TMap<TWeakObjectPtr<ACoinActor>, FCoinCardModifiers> CoinMods;

private:
    // ===== [카드 효과 내부 루틴] =====
    void CollectCoinsOnField(TArray<FCoinOnGridInfo>& OutCoins) const;

    // 카드 ID -> 로직 함수 테이블 (Initialize 시 1회 빌드)
    TMap<int32, FCardLogicFn> CardLogicTable;

    // 프로모션 카드 빛나는 그리드 (GridX==-1 이면 미선택)
    FGridPoint PromotionHighlightedGrid;

private:
    //카드 UI 비활성화
    void UnActiveCardUI();

};
