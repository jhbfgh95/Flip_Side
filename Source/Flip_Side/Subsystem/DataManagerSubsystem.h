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
#include "KeywordDataTypes.h"

#include "SQLiteDatabase.h"
#include "SQLitePreparedStatement.h"

#include "DataManagerSubsystem.generated.h"

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

    TMap<int32, TArray<FBossPatternDisplayData>> BossPatternDisplayByBossID;

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

    // 슬롯 레벨(1~3)별 비용/HP
    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FCoinSlotLevelTier> CoinSlotLevelTierByLevel;

    // 무기 설명 토큰([KW:Code])이 참조하는 키워드 정의. KeywordCode 기준 캐시.
    UPROPERTY(BlueprintReadOnly)
    TMap<FName, FKeywordDefinitionData> KeywordDefinitionByCode;

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

    // 슬롯 구조체의 Level로 해당 레벨의 구매 비용/HP를 조회
    UFUNCTION(BlueprintCallable)
    bool GetCoinSlotLevelStats(const FCoinTypeStructure& CoinSlot, int32& OutCost, int32& OutHP) const;

    // 활성 키워드 전체를 한 번에 로드 (UI 포매터가 캐시할 때 사용)
    UFUNCTION(BlueprintCallable)
    bool GetAllEnabledKeywordDefinitions(TArray<FKeywordDefinitionData>& OutDefinitions) const;

    // 단일 코드 조회 (도감·디버그·검증용)
    UFUNCTION(BlueprintCallable)
    bool TryGetKeywordByCode(FName KeywordCode, FKeywordDefinitionData& OutDefinition, bool bIncludeDisabled = false) const;

    // 무기 설명 저장/로드 시 알 수 없는 토큰과 비활성 키워드를 검출
    UFUNCTION(BlueprintCallable)
    bool ValidateWeaponDescriptionTokens(const FString& RawDescription, TArray<FString>& OutErrors) const;

private:
    bool bCacheReady = false;
    FSQLiteDatabase Db;

    void ClearCache();

    // DB ���� ���: Content/Database/DB.sqlite �� Saved�� �����ؼ� ���� ���
    bool OpenDbReadWrite();
    void CloseDb();

    bool LoadWeapons();
    bool LoadWeaponTypes();
    bool LoadBossDisplayData();
    bool LoadBossPatternDisplay();
    bool LoadItems();
    bool LoadCards();
    bool LoadStageRewards();
    bool LoadGameConfig();
    bool LoadCoinSlotLevelTiers();
    bool LoadKeywordDefinitions();

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

