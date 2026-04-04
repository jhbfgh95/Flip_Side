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

static FString GetColTextUTF8(FSQLitePreparedStatement& Stmt, int32 Index)
{
    TArray<uint8> RawBytes;
    if (Stmt.GetColumnValueByIndex(Index, RawBytes) && RawBytes.Num() > 0)
    {
        RawBytes.Add('\0'); 
        
        return FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(RawBytes.GetData())));
    }
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
        // TypeID �������� �ڵ� �׷���
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

bool UDataManagerSubsystem::TryGetAllItems(TArray<FItemData>& OutItems) const
{
    OutItems = Items;
    return true;
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

bool UDataManagerSubsystem::TryGetAllCards(TArray<FCardData>& OutCards) const
{
    OutCards = Cards;
    return true;
}

void UDataManagerSubsystem::ClearCache()
{
    WeaponByID.Reset();
    WeaponIDsByClass.Reset();
    BossByID.Reset();
    BossIDByStage.Reset();
    ItemByID.Reset();
    WeaponTypes.Reset();
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
    const TCHAR* Sql = TEXT(R"SQL(
        SELECT
            c.id,
            w.weapon_type AS weapon_type,
            c.behavior_point,
            c.range_x,
            c.range_y,
            c.icon_path,
            c.behavior,
            c.vfx_path,
            c.type_id,
            w.HP,
            c.weapon_point,
            c.weapon_name,
            c.KOR_DES,
            c.ENG_DES,
            w.typecolor,

            IFNULL(a.pattern,      8) AS pattern,       -- 기본: SingleCell
            IFNULL(a.anchor_dx,    0) AS anchor_dx,
            IFNULL(a.anchor_dy,    0) AS anchor_dy,
            IFNULL(a.anchor_mode,  0) AS anchor_mode,   -- 기본: UseAnchorCell
            IFNULL(a.param_a,      1) AS param_a,
            IFNULL(a.param_b,      1) AS param_b,
            IFNULL(a.side,         0) AS side,          -- 기본: Up
            IFNULL(a.flags,        0) AS flags,
            IFNULL(a.action_repeat_type,  0) AS action_repeat_type

        FROM coin_weapon_def AS c
        JOIN weapon_type AS w
            ON c.type_id = w.type_id
        LEFT JOIN coin_weapon_attack_area AS a
            ON a.weapon_id = c.id;
    )SQL");

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadWeapons: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FFaceData Data;
        int32 Col = 0;

        Data.WeaponID = GetColInt(Stmt, Col++);

        const FString ClassStr = GetColText(Stmt, Col++);
        Data.WeaponType = WeaponClassFromString(ClassStr);

        Data.BehaviorPoint = GetColInt(Stmt, Col++);

        // 기존 range_x/y (이거 어떻게 할지..)
        Data.AttackRange.GridX = GetColDouble(Stmt, Col++);
        Data.AttackRange.GridY = GetColDouble(Stmt, Col++);

        const FString IconPath = GetColText(Stmt, Col++);
        if (!IconPath.IsEmpty())
        {
            Data.WeaponIcon = LoadObject<UTexture2D>(nullptr, *IconPath);
        }

        Data.BehaviorCode = GetColText(Stmt, Col++);

        const FString VfxPath = GetColText(Stmt, Col++);
        if (!VfxPath.IsEmpty())
        {
            Data.WeaponVFX = LoadObject<UNiagaraSystem>(nullptr, *VfxPath);
        }

        Data.TypeID = GetColInt(Stmt, Col++);
        Data.HP = GetColInt(Stmt, Col++);
        Data.AttackPoint = GetColInt(Stmt, Col++);

        Data.WeaponName = GetColTextUTF8(Stmt, Col++);

        Data.KOR_DES = GetColTextUTF8(Stmt, Col++);
        Data.ENG_DES = GetColText(Stmt, Col++);

        const FString ColorHex = GetColText(Stmt, Col++);
        if (!TryParseHexColor_RRGGBBAA(ColorHex, Data.TypeColor))
        {
            Data.TypeColor = FLinearColor::White;
        }

        const int32 Pattern = GetColInt(Stmt, Col++);
        const int32 AnchorDX = GetColInt(Stmt, Col++);
        const int32 AnchorDY = GetColInt(Stmt, Col++);
        const int32 AnchorMode = GetColInt(Stmt, Col++);
        const int32 ParamA = GetColInt(Stmt, Col++);
        const int32 ParamB = GetColInt(Stmt, Col++);
        const int32 Side = GetColInt(Stmt, Col++);
        const int32 Flags = GetColInt(Stmt, Col++);
        const int32 ActionRepeatType = GetColInt(Stmt, Col++);

        Data.AttackAreaSpec.Pattern = (EAttackAreaPattern)Pattern;
        Data.AttackAreaSpec.AnchorMode = (EAreaAnchor)AnchorMode;
        Data.AttackAreaSpec.ParamA = ParamA;
        Data.AttackAreaSpec.ParamB = ParamB;
        Data.AttackAreaSpec.Side = (EAreaSide)Side;
        // AnchorCell(중심점의 절대좌표) 정한 뒤 오프셋 구현 단계서 사용해야 될듯?
        Data.AttackAnchorOffset = FIntPoint(AnchorDX, AnchorDY);

        Data.AttackAreaFlags = Flags;
        Data.ActionRepeatType = (EActionRepeatType)ActionRepeatType;

        if (Data.AttackAreaSpec.Pattern == EAttackAreaPattern::SingleCell)
        {
            Data.SingleCellWeaponType = (ESingleCellWeaponType)ParamB;
        }

        WeaponByID.Add(Data.WeaponID, Data);
        WeaponByTypeID.FindOrAdd(Data.TypeID).Add(Data);
        WeaponIDsByClass.FindOrAdd(Data.WeaponType).WeaponIDs.Add(Data.WeaponID);
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
        TEXT("SELECT i.item_id, i.item_range, i.item_effect_value, i.icon_path, i.item_description AS item_des, i.item_type_id, i.behavior, t.item_type_color, i.item_name FROM item i JOIN item_type t ON i.item_type_id = t.item_type_id;");

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
        if (Item.ItemTypeID == 1)      Item.ItemType = EItemType::Buff;
        else if (Item.ItemTypeID == 2) Item.ItemType = EItemType::Install;
        Item.BehaviorCode = GetColText(Stmt, 6);
        const FString ColorHex = GetColText(Stmt, 7);
        if (!TryParseHexColor_RRGGBBAA(ColorHex, Item.TypeColor))
        {
            Item.TypeColor = FLinearColor::White;
        }
        Item.ItemName = GetColTextUTF8(Stmt, 8);
        ItemByID.Add(Item.ItemID, Item);
        Items.Add(Item);
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
        Cards.Add(Card);
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

    const FString AARRGGBB = R + G + B + A;

    const FColor SRGB = FColor::FromHex(AARRGGBB);
    Out = FLinearColor::FromSRGBColor(SRGB);
    return true;
}

