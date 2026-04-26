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
class ACoinActor;

struct FCoinOnGridInfo; // GridTypes �ʿ� ���� ���ɼ� ����
struct FFaceData;
struct FGridPoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStageHandCardSet, int32, HandIndex, FCardData, CardInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStageHandCardCleared, int32, HandIndex);


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
    // ===== [���� ���� ���� API ����] =====
    UFUNCTION(BlueprintCallable)
    void RefreshHandFromGI();

    UFUNCTION(BlueprintCallable)
    void RemoveHandCard(int32 HandIndex);

    UFUNCTION(BlueprintCallable)
    bool TryGetHandCard(int32 HandIndex, FCardData& Out) const;

    

public:
    // ===== [�߰�: ī�� ȿ�� ���� API] =====
    // BattleManager/�� �ý����� Ÿ�̹� ���� ȣ���ϴ� ����(����)
    void ExecuteCardsEffect();

    void ClearPromotionHighlight();

    UFUNCTION(BlueprintCallable)
    FCoinCardModifiers GetModifiersForCoin(ACoinActor* Coin) const;

    UFUNCTION(BlueprintCallable)
    void ClearAllModifiers();

private:
    static constexpr int32 HandCount = 3;

    // ����(����) ������
    UPROPERTY()
    TArray<FCardData> HandCards;

    UPROPERTY()
    TArray<bool> bHasCard;

private:
    // ===== [���� UI ���� ����] =====
    void EnsureStageHUD(UWorld& InWorld);

    // CardID -> FCardData
    bool TryLoadCardData(int32 CardID, FCardData& Out) const;

    void ClearSlot(int32 HandIndex, bool bNotify);

private:
    UPROPERTY() UGridManagerSubsystem* GridSubsys = nullptr;
    UPROPERTY() UCrossingLevelGISubsystem* CrossingGI = nullptr;
    UPROPERTY() UDataManagerSubsystem* DM = nullptr;

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
    // HUD ���� Ŭ����(������/���ÿ��� ����)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> StageHUDClass;

    UPROPERTY()
    UUserWidget* StageHUDInstance = nullptr;
};