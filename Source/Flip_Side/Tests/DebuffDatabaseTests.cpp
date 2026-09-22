#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/GameInstance.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Actors/DebuffComponent.h"
#include "DataTypes/BossDataTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDebuffDatabaseTest, "FlipSide.Debuff.Database",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDebuffDatabaseTest::RunTest(const FString& Parameters)
{
    UGameInstance* GameInstance = NewObject<UGameInstance>();
    UDataManagerSubsystem* Manager = NewObject<UDataManagerSubsystem>(GameInstance);
    if (!TestTrue(TEXT("Existing loaders and debuff loader succeed"), Manager->ReloadCache())) return false;
    for (int32 ID : { DebuffTypeID::Blind, DebuffTypeID::Stun, BossBuffTypeID::SwampAttackPowerDown, BossBuffTypeID::SwampWeaponPowerDown })
    {
        FDebuffDefinitionData Data;
        TestTrue(TEXT("Runtime ID resolves to database"), Manager->TryGetDebuff(ID, Data));
        TestEqual(TEXT("Identity preserved"), Data.BuffTypeID, ID);
        TestFalse(TEXT("Name decoded"), Data.DisplayName.IsEmpty());
        TestFalse(TEXT("Description decoded"), Data.Description.IsEmpty());
    }
    FDebuffDefinitionData Blind;
    Manager->TryGetDebuff(DebuffTypeID::Blind, Blind);
    TestEqual(TEXT("Korean name"), Blind.DisplayName.ToString(), FString(TEXT("실명")));
    TestFalse(TEXT("Missing ID returns false"), Manager->TryGetDebuff(INDEX_NONE, Blind));
    TestTrue(TEXT("Missing ID clears stale data"), Blind.DisplayName.IsEmpty());
    TArray<FDebuffDefinitionData> All;
    TestTrue(TEXT("All definitions API"), Manager->TryGetAllDebuffs(All));
    const int32 Count = All.Num();
    TestTrue(TEXT("Reload succeeds"), Manager->ReloadCache());
    Manager->TryGetAllDebuffs(All);
    TestEqual(TEXT("Reload does not duplicate rows"), All.Num(), Count);
    Manager->Deinitialize();
    TestFalse(TEXT("Deinitialize clears cache"), Manager->TryGetDebuff(DebuffTypeID::Blind, Blind));
    return true;
}
#endif
