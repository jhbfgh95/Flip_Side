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

#include "SQLiteDatabase.h"
#include "SQLitePreparedStatement.h"

#include "DataManagerSubsystem.generated.h"

UCLASS()
class FLIP_SIDE_API UDataManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ===== 캐시 =====
    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FFaceData> WeaponByID;

    TMap<int32, TArray<FFaceData>> WeaponByTypeID;

    TArray<FWeaponType> WeaponTypes;

    UPROPERTY(BlueprintReadOnly)
    TMap<EWeaponClass, FWeaponIDArray> WeaponIDsByClass;

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FBossData> BossByID;

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, int32> BossIDByStage; // stage -> boss_id

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FItemData> ItemByID;

    UPROPERTY(BlueprintReadOnly)
    TArray<FItemData> Items;

    UPROPERTY(BlueprintReadOnly)
    TMap<int32, FCardData> CardByID;

    UPROPERTY(BlueprintReadOnly)
    TArray<FCardData> Cards;

    // ===== Subsystem =====
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    bool IsCacheReady() const { return bCacheReady; }

    UFUNCTION(BlueprintCallable)
    bool ReloadCache();

    // 조회 (선택)
    UFUNCTION(BlueprintCallable)
    bool TryGetWeapon(int32 WeaponID, FFaceData& Out) const;

    bool TryGetWeaponsByType(int32 TypeID, const TArray<FFaceData>*& OutWeapons) const;

    void BuildWeaponTypeMap(const TArray<FFaceData>& AllWeapons);

    UFUNCTION(BlueprintCallable)
    bool TryGetBossByStage(int32 Stage, FBossData& Out) const;

    UFUNCTION(BlueprintCallable)
    bool TryGetItem(int32 ItemID, FItemData& Out) const;
    bool TryGetAllItems(TArray<FItemData>& OutItems) const;

    bool TryGetCard(int32 CardID, FCardData& Out) const;
    bool TryGetAllCards(TArray<FCardData>& OutCards) const;

private:
    bool bCacheReady = false;
    FSQLiteDatabase Db;

    void ClearCache();

    // DB 파일 경로: Content/Database/DB.sqlite 를 Saved로 복사해서 여는 방식
    bool OpenDbReadWrite();
    void CloseDb();

    bool LoadWeapons();
    bool LoadWeaponTypes();
    bool LoadBosses();
    bool LoadItems();
    bool LoadCards();

    // ===== PrepareStatement 시그니처 차이 자동 대응 래퍼 =====
    template<typename TDb>
    static auto PrepareStmtImpl(TDb& InDb, const TCHAR* Sql, FSQLitePreparedStatement& OutStmt, int)
        -> decltype(InDb.PrepareStatement(Sql, OutStmt), bool())
    {
        return InDb.PrepareStatement(Sql, OutStmt);
    }

    template<typename TDb>
    static bool PrepareStmtImpl(TDb& InDb, const TCHAR* Sql, FSQLitePreparedStatement& OutStmt, long)
    {
        // PrepareStatement가 "리턴"하는 엔진 버전 대응
        OutStmt = InDb.PrepareStatement(Sql);
        return OutStmt.IsValid();
    }

    static bool PrepareStmt(FSQLiteDatabase& InDb, const TCHAR* Sql, FSQLitePreparedStatement& OutStmt)
    {
        return PrepareStmtImpl(InDb, Sql, OutStmt, 0);
    }

    // 문자열(EWeaponClass) 변환
    static EWeaponClass WeaponClassFromString(const FString& S);
};

