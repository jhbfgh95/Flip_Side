#include "DataManagerSubsystem.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Engine/Texture2D.h"
#include "NiagaraSystem.h"

static bool TryParseHexColor_RRGGBBAA(const FString& InHex, FLinearColor& OutColor);

static FString GetColText(FSQLitePreparedStatement& Stmt, int32 Index)
{
    FString Out;
    Stmt.GetColumnValueByIndex(Index, Out);
    return Out;
}

static int32 GetColInt(FSQLitePreparedStatement& Stmt, int32 Index)
{
    int32 Out = 0;
    Stmt.GetColumnValueByIndex(Index, Out);
    return Out;
}

static double GetColDouble(FSQLitePreparedStatement& Stmt, int32 Index)
{
    double Out = 0.0;
    Stmt.GetColumnValueByIndex(Index, Out);
    return Out;
}

void UDataManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ReloadCache();
}

void UDataManagerSubsystem::Deinitialize()
{
    CloseDb();
    ClearCache();
    Super::Deinitialize();
}

bool UDataManagerSubsystem::ReloadCache()
{
    bCacheReady = false;

    ClearCache();
    CloseDb();

    if (!OpenDbReadWrite())
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] OpenDbReadWrite failed"));
        return false;
    }

    bool bOk = true;
    bOk &= LoadWeapons();
    bOk &= LoadBosses();
    bOk &= LoadItems();
    bOk &= LoadWeaponTypes();
    bOk &= LoadCards();

    bCacheReady = bOk;

    UE_LOG(LogTemp, Log, TEXT("[DB] CacheReady=%s / Weapons=%d / Boss=%d / Items=%d"),
        bCacheReady ? TEXT("true") : TEXT("false"),
        WeaponByID.Num(),
        BossByID.Num(),
        ItemByID.Num());

    return bOk;
}

bool UDataManagerSubsystem::TryGetWeapon(int32 WeaponID, FFaceData& Out) const
{
    if (const FFaceData* Found = WeaponByID.Find(WeaponID))
    {
        Out = *Found;
        return true;
    }
    return false;
}
bool UDataManagerSubsystem::TryGetWeaponsByType(
    int32 TypeID,
    const TArray<FFaceData>*& OutWeapons
) const
{
    OutWeapons = WeaponByTypeID.Find(TypeID);
    return OutWeapons != nullptr;
}


void UDataManagerSubsystem::BuildWeaponTypeMap(const TArray<FFaceData>& AllWeapons)
{
    WeaponByTypeID.Empty();

    for (const FFaceData& Weapon : AllWeapons)
    {
        // TypeID 기준으로 자동 그룹핑
        WeaponByTypeID.FindOrAdd(Weapon.TypeID).Add(Weapon);
    }
}


bool UDataManagerSubsystem::TryGetBossByStage(int32 Stage, FBossData& Out) const
{
    const int32* BossIDPtr = BossIDByStage.Find(Stage);
    if (!BossIDPtr) return false;

    if (const FBossData* Found = BossByID.Find(*BossIDPtr))
    {
        Out = *Found;
        return true;
    }
    return false;
}

bool UDataManagerSubsystem::TryGetItem(int32 ItemID, FItemData& Out) const
{
    if (const FItemData* Found = ItemByID.Find(ItemID))
    {
        Out = *Found;
        return true;
    }
    return false;
}

bool UDataManagerSubsystem::TryGetCard(int32 CardID, FCardData& Out) const
{
    if (const FCardData* Found = CardByID.Find(CardID))
    {
        Out = *Found;
        return true;
    }
    return false;
}

void UDataManagerSubsystem::ClearCache()
{
    WeaponByID.Reset();
    WeaponIDsByClass.Reset();
    BossByID.Reset();
    BossIDByStage.Reset();
    ItemByID.Reset();
}

bool UDataManagerSubsystem::OpenDbReadWrite()
{
    const FString ContentDbPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("DB.db"));

    const FString SavedDbPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DB.db"));

    if (!FPaths::FileExists(SavedDbPath))
    {
        if (!FPaths::FileExists(ContentDbPath))
        {
            UE_LOG(LogTemp, Error, TEXT("[DB] DB file not found: %s"), *ContentDbPath);
            return false;
        }

        const int32 CopyResult = IFileManager::Get().Copy(*SavedDbPath, *ContentDbPath);
        if (CopyResult != COPY_OK)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DB] Copy to Saved failed. Try open Content read-only."));
            return Db.Open(*ContentDbPath, ESQLiteDatabaseOpenMode::ReadOnly);
        }
    }

    return Db.Open(*ContentDbPath, ESQLiteDatabaseOpenMode::ReadWrite);
}

void UDataManagerSubsystem::CloseDb()
{
    if (Db.IsValid())
    {
        Db.Close();
    }
}

EWeaponClass UDataManagerSubsystem::WeaponClassFromString(const FString& S)
{
    if (S.Equals(TEXT("Tank"), ESearchCase::IgnoreCase)) return EWeaponClass::Tank;
    if (S.Equals(TEXT("Deal"), ESearchCase::IgnoreCase)) return EWeaponClass::Deal;
    if (S.Equals(TEXT("Heal"), ESearchCase::IgnoreCase)) return EWeaponClass::Heal;
    return EWeaponClass::None;
}


bool UDataManagerSubsystem::LoadWeapons()
{
    const TCHAR* Sql =
        TEXT("SELECT c.id, w.weapon_type AS weapon_type, c.behavior_point, c.range_x, c.range_y, c.icon_path, c.behavior, c.vfx_path, c.type_id, w.HP, c.weapon_point, c.KOR_DES, c.ENG_DES, w.typecolor FROM coin_weapon_def AS c JOIN weapon_type AS w ON c.type_id = w.type_id; ");

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadWeapons: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FFaceData Data;

        Data.WeaponID = GetColInt(Stmt, 0);

        const FString ClassStr = GetColText(Stmt, 1);
        Data.WeaponType = WeaponClassFromString(ClassStr);

        Data.BehaviorPoint = GetColInt(Stmt, 2);

        Data.AttackRange.GridX = GetColDouble(Stmt, 3);
        Data.AttackRange.GridY = GetColDouble(Stmt, 4);

        const FString IconPath = GetColText(Stmt, 5);
        if (!IconPath.IsEmpty())
        {
            Data.WeaponIcon = LoadObject<UTexture2D>(nullptr, *IconPath);
        }

        Data.BehaviorCode = GetColText(Stmt, 6);

        const FString VfxPath = GetColText(Stmt, 7);
        if (!VfxPath.IsEmpty())
        {
            Data.WeaponVFX = LoadObject<UNiagaraSystem>(nullptr, *VfxPath);
        }

        Data.TypeID = GetColInt(Stmt, 8);
        Data.HP = GetColInt(Stmt, 9);
        Data.AttackPoint = GetColInt(Stmt, 10);

        const FString KORStr = GetColText(Stmt, 11);
        Data.KOR_DES = KORStr;

        const FString ENGStr = GetColText(Stmt, 12);
        Data.KOR_DES = KORStr;


        const FString ColorHex = GetColText(Stmt, 13);
        if (!TryParseHexColor_RRGGBBAA(ColorHex, Data.TypeColor))
        {
            Data.TypeColor = FLinearColor::White;
        }

        WeaponByID.Add(Data.WeaponID, Data);
        WeaponByTypeID.FindOrAdd(Data.TypeID).Add(Data);
        WeaponIDsByClass
            .FindOrAdd(Data.WeaponType)
            .WeaponIDs
            .Add(Data.WeaponID);
    }

    Stmt.Destroy();
    return true;
}

bool UDataManagerSubsystem::LoadWeaponTypes()
{
    const TCHAR* Sql =
        TEXT("SELECT type_id, weapon_type, HP, typecolor FROM weapon_type; ");

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadWeapons: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FWeaponType Data;

        Data.TypeID = GetColInt(Stmt, 0);

        const FString ClassStr = GetColText(Stmt, 1);
        Data.WeaponType = WeaponClassFromString(ClassStr);

        
        Data.HP = GetColInt(Stmt, 2);
        

        const FString ColorHex = GetColText(Stmt, 3);
        if (!TryParseHexColor_RRGGBBAA(ColorHex, Data.TypeColor))
        {
            Data.TypeColor = FLinearColor::White;
        }

        WeaponTypes.Add(Data);
    }

    Stmt.Destroy();
    return true;
}

bool UDataManagerSubsystem::LoadBosses()
{
    const TCHAR* Sql =
        TEXT("SELECT boss_id, boss_stage, attack_point, boss_hp "
            "FROM boss_def;");

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadBosses: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FBossData Boss;
        Boss.BossID = GetColInt(Stmt, 0);
        Boss.BossStage = GetColInt(Stmt, 1);
        Boss.AttackPoint = GetColInt(Stmt, 2);
        Boss.BossHP = GetColInt(Stmt, 3);

        BossByID.Add(Boss.BossID, Boss);
        BossIDByStage.Add(Boss.BossStage, Boss.BossID);
    }

    Stmt.Destroy();
    return true;
}

bool UDataManagerSubsystem::LoadItems()
{
    const TCHAR* Sql =
        TEXT("SELECT i.item_id, i.item_range, i.item_effect_value, i.icon_path, i.item_description AS item_des, i.item_type_id, i.behavior, t.item_type_color FROM item i JOIN item_type t ON i.item_type_id = t.item_type_id;");

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadItems: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FItemData Item;
        Item.ItemID = GetColInt(Stmt, 0);
        Item.ItemRange = GetColInt(Stmt, 1);
        Item.ItemEffectValue = GetColInt(Stmt, 2);
        const FString IconPath = GetColText(Stmt, 3);
        if (!IconPath.IsEmpty())
        {
            Item.ItemIcon = LoadObject<UTexture2D>(nullptr, *IconPath);
        }
        const FString itemdes = GetColText(Stmt, 4);
        Item.Item_DES = itemdes;
        Item.ItemTypeID = GetColInt(Stmt, 5);
        Item.BehaviorCode = GetColText(Stmt, 6);
        const FString ColorHex = GetColText(Stmt, 7);
        if (!TryParseHexColor_RRGGBBAA(ColorHex, Item.TypeColor))
        {
            Item.TypeColor = FLinearColor::White;
        }
        ItemByID.Add(Item.ItemID, Item);
    }

    Stmt.Destroy();
    return true;
}

bool UDataManagerSubsystem::LoadCards()
{
    const TCHAR* Sql =
        TEXT("SELECT CardID, icon_path, CardName, Card_Description ")
        TEXT("FROM Card;");

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadCards: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FCardData Card;

        Card.CardID = GetColInt(Stmt, 0);

        const FString IconPath = GetColText(Stmt, 1);
        Card.CardName = GetColText(Stmt, 2);
        Card.Card_Description = GetColText(Stmt, 3);


        CardByID.Add(Card.CardID, Card);
    }

    Stmt.Destroy();
    return true;
}

static bool TryParseHexColor_RRGGBBAA(const FString& InHex, FLinearColor& Out)
{
    FString Hex = InHex;
    Hex.TrimStartAndEndInline();
    Hex.RemoveFromStart(TEXT("#"));

    if (Hex.Len() == 6) Hex += TEXT("FF");
    if (Hex.Len() != 8) return false;

    const FString R = Hex.Mid(0, 2);
    const FString G = Hex.Mid(2, 2);
    const FString B = Hex.Mid(4, 2);
    const FString A = Hex.Mid(6, 2);

    const FString AARRGGBB = A + R + G + B;

    const FColor SRGB = FColor::FromHex(AARRGGBB);
    Out = FLinearColor::FromSRGBColor(SRGB);
    return true;
}

