#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "WeaponDataTypes.h"
#include "BossDataTypes.h"
#include "ItemDataTypes.h"
#include "CoinDataTypes.h"
#include "FlipSide_Enum.h"
#include "GridTypes.h"
#include "CardTypes.h"
#include "GameConfigTypes.h"

#include "SQLiteDatabase.h"
#include "SQLitePreparedStatement.h"

#include "DataManagerSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FBossPatternDisplayDataArray
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TArray<FBossPatternDisplayData> PatternDataList;
};

UCLASS()
class FLIP_SIDE_API UDataManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ===== ĳ�� =====
    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FFaceData> WeaponByID;

    TMap<int32, TArray<FFaceData>> WeaponByTypeID;

    TArray<FWeaponType> WeaponTypes;

    UPROPERTY(BlueprintReadOnly)
    TMap<EWeaponClass, FWeaponIDArray> WeaponIDsByClass;

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FBossDisplayData> BossByID;

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, int32> BossIDByStage;

    // stage_id → (boss_id, encounter)

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FBossPatternDisplayDataArray> BossPatternDisplayByBossID;

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FItemData> ItemByID;

    UPROPERTY(BlueprintReadOnly)
    TArray<FItemData> Items;

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FCardData> CardByID;

    UPROPERTY(BlueprintReadOnly)
    TArray<FCardData> Cards;

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FStageRewardData> StageRewardByStageID;

    UPROPERTY(BlueprintReadOnly)
    FGameConfigData GameConfig;

    // ===== Subsystem =====
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    bool IsCacheReady() const { return bCacheReady; }

    UFUNCTION(BlueprintCallable)
    bool ReloadCache();

    // ��ȸ (����)
    UFUNCTION(BlueprintCallable)
    bool TryGetWeapon(int32 WeaponID, FFaceData& Out) const;

    bool TryGetWeaponsByType(int32 TypeID, const TArray<FFaceData>*& OutWeapons) const;

    void BuildWeaponTypeMap(const TArray<FFaceData>& AllWeapons);

    UFUNCTION(BlueprintCallable)
    bool TryGetBossByStage(int32 Stage, FBossDisplayData& Out) const;

    UFUNCTION(BlueprintCallable)
    bool TryGetBossPatternDisplay(int32 BossID, TArray<FBossPatternDisplayData>& Out) const;

    bool LoadBossBattleData(int32 BossID, FBossBattleData& Out);

    bool TryGetStageMultiplier(int32 BossID, int32 Stage, float& OutStatMultiplier, float& OutGimmickMultiplier);


    UFUNCTION(BlueprintCallable)
    bool TryGetItem(int32 ItemID, FItemData& Out) const;
    bool TryGetAllItems(TArray<FItemData>& OutItems) const;

    bool TryGetCard(int32 CardID, FCardData& Out) const;
    bool TryGetAllCards(TArray<FCardData>& OutCards) const;

    UFUNCTION(BlueprintCallable)
    bool TryGetStageReward(int32 StageID, FStageRewardData& Out) const;

private:
    bool bCacheReady = false;
    FSQLiteDatabase Db;

    void ClearCache();

    // DB ���� ���: Content/Database/DB.sqlite �� Saved�� �����ؼ� ���� ���
    bool OpenDbReadOnly();
    void CloseDb();

    bool LoadWeapons();
    bool LoadWeaponTypes();
    bool LoadBossDisplayData();
    bool LoadBossPatternDisplay();
    bool LoadItems();
    bool LoadCards();
    bool LoadStageRewards();
    bool LoadGameConfig();

    static EAttackAreaPattern AttackAreaPatternFromInt(int32 Val);
    static EAreaAnchor AreaAnchorFromInt(int32 Val);
    static EAreaSide AreaSideFromInt(int32 Val);

    // ===== PrepareStatement �ñ״�ó ���� �ڵ� ���� ���� =====
    template<typename TDb>
    static auto PrepareStmtImpl(TDb& InDb, const TCHAR* Sql, FSQLitePreparedStatement& OutStmt, int)
        -> decltype(InDb.PrepareStatement(Sql, OutStmt), bool())
    {
        return InDb.PrepareStatement(Sql, OutStmt);
    }

    template<typename TDb>
    static bool PrepareStmtImpl(TDb& InDb, const TCHAR* Sql, FSQLitePreparedStatement& OutStmt, long)
    {
        // PrepareStatement�� "����"�ϴ� ���� ���� ����
        OutStmt = InDb.PrepareStatement(Sql);
        return OutStmt.IsValid();
    }

    static bool PrepareStmt(FSQLiteDatabase& InDb, const TCHAR* Sql, FSQLitePreparedStatement& OutStmt)
    {
        return PrepareStmtImpl(InDb, Sql, OutStmt, 0);
    }

    // ���ڿ�(EWeaponClass) ��ȯ
    static EWeaponClass WeaponClassFromString(const FString& S);
};

