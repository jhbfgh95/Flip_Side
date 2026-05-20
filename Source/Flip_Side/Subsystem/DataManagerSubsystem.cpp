#include "DataManagerSubsystem.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Engine/Texture2D.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"

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

    if (!OpenDbReadOnly())
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] OpenDbReadOnly failed"));
        return false;
    }

    bool bOk = true;
    bOk &= LoadWeapons();
    bOk &= LoadBossDisplayData();
    bOk &= LoadBossPatternDisplay();
    bOk &= LoadItems();
    bOk &= LoadWeaponTypes();
    bOk &= LoadCards();
    bOk &= LoadStageRewards();
    bOk &= LoadGameConfig();

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


bool UDataManagerSubsystem::TryGetBossByStage(int32 Stage, FBossDisplayData& Out) const
{
    const int32* BossIDPtr = BossIDByStage.Find(Stage);
    if (!BossIDPtr) return false;

    if (const FBossDisplayData* Found = BossByID.Find(*BossIDPtr))
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

bool UDataManagerSubsystem::TryGetStageReward(int32 StageID, FStageRewardData& Out) const
{
    if (const FStageRewardData* Found = StageRewardByStageID.Find(StageID))
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
    BossPatternDisplayByBossID.Reset();
    ItemByID.Reset();
    WeaponTypes.Reset();
    StageRewardByStageID.Reset();
    GameConfig = FGameConfigData();
}

bool UDataManagerSubsystem::OpenDbReadOnly()
{
    const FString ContentDbPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Database/DB.db"));

    if (!FPaths::FileExists(ContentDbPath))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] DB file not found: %s"), *ContentDbPath);
        return false;
    }

    return Db.Open(*ContentDbPath, ESQLiteDatabaseOpenMode::ReadOnly);
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
            IFNULL(c.vfx_target, 0) AS vfx_target,
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
            IFNULL(a.action_repeat_type,  0) AS action_repeat_type,
            IFNULL(c.price,        0) AS price,
            c.sfx_path

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

        Data.WeaponVFXTarget = (EWeaponVFXTarget)GetColInt(Stmt, Col++);

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

        Data.Price = GetColInt(Stmt, Col++);

        const FString SfxPath = GetColText(Stmt, Col++);
        if (!SfxPath.IsEmpty())
        {
            Data.WeaponSFX = LoadObject<USoundBase>(nullptr, *SfxPath);
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

EAttackAreaPattern UDataManagerSubsystem::AttackAreaPatternFromInt(int32 Val)
{
    return static_cast<EAttackAreaPattern>(FMath::Clamp(Val, 0, (int32)EAttackAreaPattern::RectFromCell));
}

EAreaAnchor UDataManagerSubsystem::AreaAnchorFromInt(int32 Val)
{
    return static_cast<EAreaAnchor>(FMath::Clamp(Val, 0, 1));
}

EAreaSide UDataManagerSubsystem::AreaSideFromInt(int32 Val)
{
    return static_cast<EAreaSide>(FMath::Clamp(Val, 0, 3));
}

bool UDataManagerSubsystem::LoadBossDisplayData()
{
    {
        const TCHAR* Sql = TEXT(
            "SELECT boss_id, boss_stage, theme_id, boss_name, boss_image_path, ability_description, attack_point, boss_hp, shield_value "
            "FROM boss_def;"
        );

        FSQLitePreparedStatement Stmt;
        if (!PrepareStmt(Db, Sql, Stmt))
        {
            UE_LOG(LogTemp, Error, TEXT("[DB] LoadBossDisplayData: PrepareStatement failed"));
            return false;
        }

        while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
        {
            FBossDisplayData Boss;
            Boss.BossID    = GetColInt(Stmt, 0);
            Boss.BossStage = GetColInt(Stmt, 1);
            Boss.ThemeID   = GetColInt(Stmt, 2);
            Boss.BossName  = GetColTextUTF8(Stmt, 3);

            const FString ImagePath = GetColText(Stmt, 4);
            if (!ImagePath.IsEmpty())
                Boss.BossImage = LoadObject<UTexture2D>(nullptr, *ImagePath);

            Boss.BossAbilityDescription = FText::FromString(GetColTextUTF8(Stmt, 5));
            Boss.AttackPoint = GetColInt(Stmt, 6);
            Boss.BossHP      = GetColInt(Stmt, 7);
            Boss.ShieldValue = GetColInt(Stmt, 8);

            BossByID.Add(Boss.BossID, Boss);
            BossIDByStage.Add(Boss.BossStage, Boss.BossID);
        }
        Stmt.Destroy();
    }

    {
        const TCHAR* Sql = TEXT(
            "SELECT boss_id, gimmick_type, param_int_a, param_int_b, param_float_a, param_float_b, param_str_a, gimmick_name, gimmick_description, shield_value, param_float_c "
            "FROM boss_gimmick;"
        );

        FSQLitePreparedStatement Stmt;
        if (!PrepareStmt(Db, Sql, Stmt)) return false;

        while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
        {
            const int32 BossID = GetColInt(Stmt, 0);
            FBossDisplayData* Found = BossByID.Find(BossID);
            if (!Found) continue;

            FBossGimmickData G;
            G.GimmickType        = static_cast<EBossGimmickType>(GetColInt(Stmt, 1));
            G.ParamIntA          = GetColInt(Stmt, 2);
            G.ParamIntB          = GetColInt(Stmt, 3);
            G.ParamFloatA        = (float)GetColDouble(Stmt, 4);
            G.ParamFloatB        = (float)GetColDouble(Stmt, 5);
            G.ParamStrA          = GetColText(Stmt, 6);
            G.GimmickName        = GetColTextUTF8(Stmt, 7);
            G.GimmickDescription = GetColTextUTF8(Stmt, 8);
            G.ShieldValue        = GetColInt(Stmt, 9);
            G.ParamFloatC        = (float)GetColDouble(Stmt, 10);
            Found->GimmickList.Add(G);
        }
        Stmt.Destroy();
    }

    return true;
}

bool UDataManagerSubsystem::LoadBossPatternDisplay()
{
    const TCHAR* Sql = TEXT(
        "SELECT p.boss_id, p.pattern_order, p.pattern_name, p.pattern_description, "
        "p.damage_ratio, "
        "p.area_pattern, p.anchor_dx, p.anchor_dy, p.anchor_mode, p.param_a, p.param_b, p.side, "
        "p.pattern_icon_path, p.pattern_range_image_path, p.shield_heal, "
        "p.gimmick_type, p.param_a, p.param_b, "
        "COALESCE(g.param_int_a, 0) AS gimmick_int_a, COALESCE(g.param_int_b, 0) AS gimmick_int_b "
        "FROM boss_pattern_def p "
        "LEFT JOIN boss_gimmick g ON p.boss_id = g.boss_id AND p.gimmick_type = g.gimmick_type "
        "ORDER BY p.boss_id, p.pattern_order;"
    );

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadBossPatternDisplay: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FBossPatternDisplayData Pattern;
        const int32 BossID         = GetColInt(Stmt, 0);
        Pattern.PatternName        = GetColTextUTF8(Stmt, 2);
        Pattern.PatternDescription = FText::FromString(GetColTextUTF8(Stmt, 3));
        Pattern.DamageRatio        = (float)GetColDouble(Stmt, 4);

        Pattern.PatternSpec.Pattern    = AttackAreaPatternFromInt(GetColInt(Stmt, 5));
        Pattern.PatternSpec.AnchorCell = FGridPoint(GetColInt(Stmt, 6), GetColInt(Stmt, 7));
        Pattern.PatternSpec.AnchorMode = AreaAnchorFromInt(GetColInt(Stmt, 8));
        Pattern.PatternSpec.ParamA     = GetColInt(Stmt, 9);
        Pattern.PatternSpec.ParamB     = GetColInt(Stmt, 10);
        Pattern.PatternSpec.Side       = AreaSideFromInt(GetColInt(Stmt, 11));

        const FString IconPath = GetColText(Stmt, 12);
        if (!IconPath.IsEmpty())
            Pattern.PatternIcon = LoadObject<UTexture2D>(nullptr, *IconPath);

        const FString RangeImagePath = GetColText(Stmt, 13);
        if (!RangeImagePath.IsEmpty())
            Pattern.PatternRangeImage = LoadObject<UTexture2D>(nullptr, *RangeImagePath);

        Pattern.ShieldHeal   = GetColInt(Stmt, 14);
        Pattern.GimmickType  = static_cast<EBossGimmickType>(GetColInt(Stmt, 15));

        // 실명(Blind=6): param_a = 지속 턴수
        // 늪(GridDebuff=2): param_a = 지속 턴수, param_b = 공격력 디버프
        // 독(Poison=5): gimmick_int_a = 독 데미지
        const int32 PatternParamA  = GetColInt(Stmt, 16);
        const int32 PatternParamB  = GetColInt(Stmt, 17);
        const int32 GimmickIntA    = GetColInt(Stmt, 18);

        if (Pattern.GimmickType == EBossGimmickType::Poison)
        {
            Pattern.GimmickParamA = GimmickIntA;
        }
        else
        {
            Pattern.GimmickParamA = PatternParamA;
            Pattern.GimmickParamB = PatternParamB;
        }

        BossPatternDisplayByBossID.FindOrAdd(BossID).PatternDataList.Add(Pattern);
    }

    Stmt.Destroy();
    return true;
}

bool UDataManagerSubsystem::TryGetBossPatternDisplay(int32 BossID, TArray<FBossPatternDisplayData>& Out) const
{
    if (const FBossPatternDisplayDataArray* Found = BossPatternDisplayByBossID.Find(BossID))
    {
        Out = Found->PatternDataList;
        return true;
    }
    return false;
}

bool UDataManagerSubsystem::LoadBossBattleData(int32 BossID, FBossBattleData& Out)
{
    if (!Db.IsValid())
    {
        if (!OpenDbReadOnly()) return false;
    }

    // boss_def
    {
        const TCHAR* Sql = TEXT(
            "SELECT boss_id, boss_stage, theme_id, boss_name, boss_image_path, "
            "attack_point, boss_hp, spawn_loc_x, spawn_loc_y, spawn_loc_z, spawn_rot_yaw, "
            "boss_class_path, pattern_class_path, stage_multiplier_stat, stage_multiplier_gimmick, shield_value, "
            "clear_anim_path, hit_anim_path "
            "FROM boss_def WHERE boss_id = ?;"
        );

        FSQLitePreparedStatement Stmt;
        if (!PrepareStmt(Db, Sql, Stmt)) return false;
        Stmt.SetBindingValueByIndex(1, BossID);

        if (Stmt.Step() != ESQLitePreparedStatementStepResult::Row)
        {
            Stmt.Destroy();
            return false;
        }

        Out.BossID    = GetColInt(Stmt, 0);
        Out.BossStage = GetColInt(Stmt, 1);
        Out.ThemeID   = GetColInt(Stmt, 2);
        Out.BossName  = GetColTextUTF8(Stmt, 3);

        const FString ImagePath = GetColText(Stmt, 4);
        if (!ImagePath.IsEmpty())
            Out.BossImage = LoadObject<UTexture2D>(nullptr, *ImagePath);

        Out.AttackPoint  = GetColInt(Stmt, 5);
        Out.BossHP       = GetColInt(Stmt, 6);
        Out.ShieldValue  = GetColInt(Stmt, 15);
        Out.SpawnLoc    = FVector(GetColDouble(Stmt, 7), GetColDouble(Stmt, 8), GetColDouble(Stmt, 9));
        Out.SpawnRot    = FRotator(0.0, GetColDouble(Stmt, 10), 0.0);

        const FString BossClassPath = GetColText(Stmt, 11);
        if (!BossClassPath.IsEmpty())
            Out.BossClass = TSoftClassPtr<ABossActor>(FSoftObjectPath(BossClassPath));

        const FString PatternClassPath = GetColText(Stmt, 12);
        if (!PatternClassPath.IsEmpty())
        {
            UClass* Loaded = Cast<UClass>(FSoftObjectPath(PatternClassPath).TryLoad());
            if (Loaded) Out.PatternClass = Loaded;
        }

        Out.StageMultiplierStat    = (float)GetColDouble(Stmt, 13);
        Out.StageMultiplierGimmick = (float)GetColDouble(Stmt, 14);

        const FString ClearAnimPath = GetColText(Stmt, 16);
        if (!ClearAnimPath.IsEmpty())
            Out.ClearAnim = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(ClearAnimPath));

        const FString HitAnimPath = GetColText(Stmt, 17);
        if (!HitAnimPath.IsEmpty())
            Out.HitAnim = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(HitAnimPath));

        Stmt.Destroy();
    }

    // boss_pattern_def
    {
        const TCHAR* Sql = TEXT(
            "SELECT pattern_name, pattern_description, damage_ratio, apply_delay, "
            "area_pattern, anchor_dx, anchor_dy, anchor_mode, param_a, param_b, side, "
            "montage_path, effect_path, effect_scale_x, effect_scale_y, effect_scale_z, "
            "visual_actor_path, pattern_icon_path, pattern_range_image_path, no_damage, gimmick_type, shield_heal, "
            "effect_target "
            "FROM boss_pattern_def WHERE boss_id = ? ORDER BY pattern_order;"
        );

        FSQLitePreparedStatement Stmt;
        if (!PrepareStmt(Db, Sql, Stmt)) return false;
        Stmt.SetBindingValueByIndex(1, BossID);

        while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
        {
            FBossPatternBattleData P;
            P.PatternName        = GetColTextUTF8(Stmt, 0);
            P.PatternDescription = FText::FromString(GetColTextUTF8(Stmt, 1));
            P.DamageRatio        = (float)GetColDouble(Stmt, 2);
            P.ApplyDelay         = (float)GetColDouble(Stmt, 3);

            P.PatternSpec.Pattern    = AttackAreaPatternFromInt(GetColInt(Stmt, 4));
            P.PatternSpec.AnchorCell = FGridPoint(GetColInt(Stmt, 5), GetColInt(Stmt, 6));
            P.PatternSpec.Index      = GetColInt(Stmt, 5); // anchor_dx → Index (UseIndex 모드용)
            P.PatternSpec.AnchorMode = AreaAnchorFromInt(GetColInt(Stmt, 7));
            P.PatternSpec.ParamA     = GetColInt(Stmt, 8);
            P.PatternSpec.ParamB     = GetColInt(Stmt, 9);
            P.PatternSpec.Side       = AreaSideFromInt(GetColInt(Stmt, 10));

            const FString MontagePath = GetColText(Stmt, 11);
            if (!MontagePath.IsEmpty())
                P.PatternMontage = LoadObject<UAnimMontage>(nullptr, *MontagePath);

            P.PatternEffect = TSoftObjectPtr<UNiagaraSystem>(FSoftObjectPath(GetColText(Stmt, 12)));
            P.PatternScale  = FVector(GetColDouble(Stmt, 13), GetColDouble(Stmt, 14), GetColDouble(Stmt, 15));
            P.VisualActorClass = TSoftClassPtr<ABase_PatternVisualActor>(FSoftObjectPath(GetColText(Stmt, 16)));

            const FString IconPath = GetColText(Stmt, 17);
            if (!IconPath.IsEmpty())
                P.PatternIcon = LoadObject<UTexture2D>(nullptr, *IconPath);

            const FString RangeImagePath = GetColText(Stmt, 18);
            if (!RangeImagePath.IsEmpty())
                P.PatternRangeImage = LoadObject<UTexture2D>(nullptr, *RangeImagePath);

            P.bNoDamage          = GetColInt(Stmt, 19) != 0;
            P.GimmickType        = static_cast<EBossGimmickType>(GetColInt(Stmt, 20));
            P.ShieldHeal         = GetColInt(Stmt, 21);
            P.PatternEffectTarget = static_cast<EBossPatternTarget>(GetColInt(Stmt, 22));

            Out.PatternList.Add(P);
        }
        Stmt.Destroy();
    }

    // boss_gimmick
    {
        const TCHAR* Sql = TEXT(
            "SELECT gimmick_type, param_int_a, param_int_b, param_float_a, param_float_b, param_str_a, gimmick_name, gimmick_description, shield_value, gimmick_class_path "
            "FROM boss_gimmick WHERE boss_id = ?;"
        );

        FSQLitePreparedStatement Stmt;
        if (!PrepareStmt(Db, Sql, Stmt)) return false;
        Stmt.SetBindingValueByIndex(1, BossID);

        while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
        {
            FBossGimmickData G;
            G.GimmickType        = static_cast<EBossGimmickType>(GetColInt(Stmt, 0));
            G.ParamIntA          = GetColInt(Stmt, 1);
            G.ParamIntB          = GetColInt(Stmt, 2);
            G.ParamFloatA        = (float)GetColDouble(Stmt, 3);
            G.ParamFloatB        = (float)GetColDouble(Stmt, 4);
            G.ParamStrA          = GetColText(Stmt, 5);
            G.GimmickName        = GetColTextUTF8(Stmt, 6);
            G.GimmickDescription = GetColTextUTF8(Stmt, 7);
            G.ShieldValue        = GetColInt(Stmt, 8);
            G.GimmickClassPath   = GetColText(Stmt, 9);
            Out.GimmickList.Add(G);
        }
        Stmt.Destroy();
    }

    // boss_background
    {
        const TCHAR* Sql = TEXT(
            "SELECT slot, texture_path FROM boss_background WHERE boss_id = ? ORDER BY slot;"
        );

        FSQLitePreparedStatement Stmt;
        if (PrepareStmt(Db, Sql, Stmt))
        {
            Stmt.SetBindingValueByIndex(1, BossID);
            Out.BackgroundTextures.SetNum(4);
            while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
            {
                const int32 Slot = GetColInt(Stmt, 0);
                const FString TexPath = GetColText(Stmt, 1);
                if (Out.BackgroundTextures.IsValidIndex(Slot) && !TexPath.IsEmpty())
                    Out.BackgroundTextures[Slot] = LoadObject<UTexture2D>(nullptr, *TexPath);
            }
            Stmt.Destroy();
        }
    }

    return true;
}

static EItemVFXTarget ItemVFXTargetFromString(const FString& S)
{
    if (S.Equals(TEXT("TargetGrid"), ESearchCase::IgnoreCase)) return EItemVFXTarget::TargetGrid;
    if (S.Equals(TEXT("TargetCoin"), ESearchCase::IgnoreCase)) return EItemVFXTarget::TargetCoin;
    if (S.Equals(TEXT("TargetOther"), ESearchCase::IgnoreCase)) return EItemVFXTarget::TargetOther;
    return EItemVFXTarget::None;
}

bool UDataManagerSubsystem::LoadItems()
{
    const TCHAR* Sql =
        TEXT("SELECT i.item_id, i.item_range, i.item_effect_value, i.icon_path, i.item_description AS item_des, i.item_type_id, i.behavior, t.item_type_color, i.item_name, IFNULL(i.price, 0), i.vfx_path, i.VFXTarget FROM item i JOIN item_type t ON i.item_type_id = t.item_type_id;");

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
        if (Item.ItemTypeID == 1)      Item.ItemType = EItemType::CoinBuff;
        else if (Item.ItemTypeID == 2) Item.ItemType = EItemType::Install;
        Item.BehaviorCode = GetColText(Stmt, 6);
        const FString ColorHex = GetColText(Stmt, 7);
        if (!TryParseHexColor_RRGGBBAA(ColorHex, Item.TypeColor))
        {
            Item.TypeColor = FLinearColor::White;
        }
        Item.ItemName = GetColTextUTF8(Stmt, 8);
        Item.Price = GetColInt(Stmt, 9);

        const FString VfxPath = GetColText(Stmt, 10);
        if (!VfxPath.IsEmpty())
        {
            Item.ItemVFX = LoadObject<UNiagaraSystem>(nullptr, *VfxPath);
        }
        Item.ItemVFXTarget = ItemVFXTargetFromString(GetColText(Stmt, 11));

        ItemByID.Add(Item.ItemID, Item);
        Items.Add(Item);
    }

    Stmt.Destroy();
    return true;
}

bool UDataManagerSubsystem::TryGetStageMultiplier(int32 BossID, int32 Stage, float& OutStatMultiplier, float& OutGimmickMultiplier)
{
    if (!Db.IsValid())
    {
        if (!OpenDbReadOnly()) return false;
    }

    const TCHAR* Sql = TEXT(
        "SELECT stat_multiplier, gimmick_multiplier FROM boss_stage_multiplier WHERE boss_id = ? AND stage = ?;"
    );

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt)) return false;

    Stmt.SetBindingValueByIndex(1, BossID);
    Stmt.SetBindingValueByIndex(2, Stage);

    if (Stmt.Step() != ESQLitePreparedStatementStepResult::Row)
    {
        Stmt.Destroy();
        OutStatMultiplier = 1.0f;
        OutGimmickMultiplier = 1.0f;
        return false;
    }

    OutStatMultiplier    = (float)GetColDouble(Stmt, 0);
    OutGimmickMultiplier = (float)GetColDouble(Stmt, 1);
    Stmt.Destroy();
    return true;
}

bool UDataManagerSubsystem::LoadCards()
{
    const TCHAR* Sql =
        TEXT("SELECT CardID, icon_path, CardName, Card_Description,")
        TEXT("       trigger_count, attack_add, behavior_add, range_add, extra_actions, b_lifesteal,")
        TEXT("       IFNULL(price, 0) ")
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

        Card.CardID           = GetColInt(Stmt, 0);
        const FString IconPath = GetColText(Stmt, 1);
        Card.CardName         = GetColTextUTF8(Stmt, 2);
        Card.Card_Description = GetColTextUTF8(Stmt, 3);
        Card.TriggerCount     = GetColInt(Stmt, 4);
        Card.AttackAdd        = GetColInt(Stmt, 5);
        Card.BehaviorAdd      = GetColInt(Stmt, 6);
        Card.RangeAdd         = GetColInt(Stmt, 7);
        Card.ExtraActions     = GetColInt(Stmt, 8);
        Card.bLifeSteal       = GetColInt(Stmt, 9) != 0;
        Card.Price            = GetColInt(Stmt, 10);

        if (!IconPath.IsEmpty())
        {
            Card.Icon = LoadObject<UTexture2D>(nullptr, *IconPath);
        }

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

bool UDataManagerSubsystem::LoadStageRewards()
{
    const TCHAR* Sql = TEXT("SELECT stage_id, reward_gold FROM stage_reward;");

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadStageRewards: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FStageRewardData Data;
        Data.StageID    = GetColInt(Stmt, 0);
        Data.RewardGold = GetColInt(Stmt, 1);
        StageRewardByStageID.Add(Data.StageID, Data);
    }

    Stmt.Destroy();
    return true;
}

bool UDataManagerSubsystem::LoadGameConfig()
{
    const TCHAR* Sql = TEXT("SELECT config_key, config_value FROM game_config;");

    FSQLitePreparedStatement Stmt;
    if (!PrepareStmt(Db, Sql, Stmt))
    {
        UE_LOG(LogTemp, Error, TEXT("[DB] LoadGameConfig: PrepareStatement failed"));
        return false;
    }

    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FString Key;
        Stmt.GetColumnValueByIndex(0, Key);
        int32 Value = GetColInt(Stmt, 1);

        if (Key == TEXT("initial_gold"))              GameConfig.InitialGold = Value;
        else if (Key == TEXT("coin_slot_unlock_cost")) GameConfig.CoinSlotUnlockCost = Value;
        else if (Key == TEXT("coin_count_add_cost"))   GameConfig.CoinCountAddCost = Value;
    }

    Stmt.Destroy();
    return true;
}

