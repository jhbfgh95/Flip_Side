#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UI/CoinDescriptionFormatter.h"
#include "SQLiteDatabase.h"
#include "SQLitePreparedStatement.h"
#include "Misc/Paths.h"

namespace CoinDescriptionTestPrivate
{
	TArray<FKeywordDefinitionData> Keywords()
	{
		TArray<FKeywordDefinitionData> Result;
		for (const TCHAR* Code : {TEXT("Attack"), TEXT("Mobility"), TEXT("Hit"), TEXT("Continuous"), TEXT("Absorb"), TEXT("Strike")})
		{
			FKeywordDefinitionData& Data = Result.AddDefaulted_GetRef();
			Data.KeywordCode = Code;
			Data.DisplayName = FText::FromString(Code);
			Data.bEnabled = true;
		}
		return Result;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCoinDescriptionSectionsTest, "FlipSide.UI.CoinDescription.Sections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCoinDescriptionSectionsTest::RunTest(const FString& Parameters)
{
	const FString Raw = TEXT("[KW:Hit]\\n[KW:Mobility] 능력의 확률이 증가합니다.\\n[KW:Attack] [KW:Strike]\\n[STAT:AttackPower] 피해\\n[KW:Mobility] [KW:Absorb] [KW:Continuous] : [STAT:Count]\\n선택 대상을 [KW:Absorb]합니다.");
	const FCoinWeaponDescriptionData Data = FCoinDescriptionFormatter::Parse(Raw, CoinDescriptionTestPrivate::Keywords(), {2, 3, 4});
	TestEqual(TEXT("Body keyword references do not create sections/bookmarks"), Data.Sections.Num(), 3);
	if (Data.Sections.Num() != 3) return false;
	TestEqual(TEXT("Attack first"), Data.Sections[0].MainKeywordCode, FName(TEXT("Attack")));
	TestEqual(TEXT("Mobility second"), Data.Sections[1].MainKeywordCode, FName(TEXT("Mobility")));
	TestEqual(TEXT("Hit third"), Data.Sections[2].MainKeywordCode, FName(TEXT("Hit")));
	TestEqual(TEXT("Two subordinate keywords stay in one section"), Data.Sections[1].AdditionalKeywordCodes.Num(), 2);
	TestEqual(TEXT("Repeat count suffix preserved for display only"), Data.Sections[1].HeaderSuffix, FString(TEXT(": [STAT:Count]")));
	TestTrue(TEXT("Main keyword reference remains in Hit body"), Data.Sections[2].Description.ToString().StartsWith(TEXT("[KW:Mobility]")));
	const auto WeaponRepeat = FCoinDescriptionFormatter::Parse(TEXT("[KW:Attack] [KW:Continuous] : [STAT:WeaponPower]\nAttack"), CoinDescriptionTestPrivate::Keywords(), {1,5,0});
	TestEqual(TEXT("Weapon power repeat remains a display suffix"), WeaponRepeat.Sections[0].HeaderSuffix, FString(TEXT(": [STAT:WeaponPower]")));
	const auto Unknown = FCoinDescriptionFormatter::Parse(TEXT("Unfinished weapon text"), {}, {});
	TestTrue(TEXT("Unclassified text does not invent a keyword bookmark"), Unknown.Sections.Num() == 1 && Unknown.Sections[0].MainKeywordCode.IsNone());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCoinDescriptionTokensTest, "FlipSide.UI.CoinDescription.Tokens",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCoinDescriptionTokensTest::RunTest(const FString& Parameters)
{
	FCoinDescriptionSectionData Data;
	Data.BaseStats = {3, 7, 2};
	FCoinDescriptionTokenData Token;
	TestTrue(TEXT("Basic attack stat exists"), FCoinDescriptionFormatter::ResolveToken(TEXT("STAT:AttackPower"), Data, Token));
	TestEqual(TEXT("Basic display is icon then base value"), FCoinDescriptionFormatter::TokenText(Token, false), FString(TEXT("<아이콘> 3")));
	TestEqual(TEXT("Shift is value then bracketed icon then parenthesized name"), FCoinDescriptionFormatter::TokenText(Token, true), FString(TEXT("3 [<아이콘>] (공격력)")));
	const auto DetailedParts = FCoinDescriptionFormatter::BuildInlineParts(Token, true);
	TestEqual(TEXT("Opening bracket follows value"), DetailedParts.BeforeIcon, FString(TEXT("3 [")));
	TestEqual(TEXT("Closing bracket precedes name"), DetailedParts.AfterIcon, FString(TEXT("] (공격력)")));
	const auto HiddenValue = FCoinDescriptionFormatter::BuildInlineParts(Token, true, false);
	TestEqual(TEXT("Unneeded numeric value omitted"), HiddenValue.BeforeIcon, FString(TEXT("[")));
	Token.Value = 0;
	TestEqual(TEXT("Zero is still a valid numeric value"), FCoinDescriptionFormatter::BuildInlineParts(Token, false).AfterIcon, FString(TEXT(" 0")));
	TestEqual(TEXT("No stat mutation"), Data.BaseStats.AttackPoint, 3);
	for (const TCHAR* Key : {TEXT("BUFF:Absorb"), TEXT("BUFF:Strike"), TEXT("STAT:AttackRange"), TEXT("STAT:AbilityRange")})
	{
		TestTrue(TEXT("Non numeric token resolves"), FCoinDescriptionFormatter::ResolveToken(Key, Data, Token));
		TestFalse(TEXT("Buff/range never shows a number"), Token.bHasValue);
		const auto Basic = FCoinDescriptionFormatter::BuildInlineParts(Token, false);
		TestTrue(TEXT("Basic non numeric entry is icon only"), Basic.BeforeIcon.IsEmpty() && Basic.AfterIcon.IsEmpty());
		const auto Detail = FCoinDescriptionFormatter::BuildInlineParts(Token, true);
		TestEqual(TEXT("Non numeric detail omits value"), Detail.BeforeIcon, FString(TEXT("[")));
		TestTrue(TEXT("Non numeric detail includes parenthesized name"), Detail.AfterIcon.StartsWith(TEXT("] (")));
	}
	FCoinDescriptionFormatter::ResolveToken(TEXT("KW:Attack"), Data, Token);
	const auto BasicKeyword = FCoinDescriptionFormatter::BuildInlineParts(Token, false);
	TestTrue(TEXT("Keywords use the same compact icon-only rule"), BasicKeyword.BeforeIcon.IsEmpty() && BasicKeyword.AfterIcon.IsEmpty());
	TestEqual(TEXT("Keyword detail includes name"), FCoinDescriptionFormatter::BuildInlineParts(Token, true).AfterIcon, FString(TEXT("] (Attack)")));
	const FString Rich = FCoinDescriptionFormatter::ToRichText(TEXT("A < B & [STAT:AttackPower], [VALUE:10]% [STAT:BossPatternAttackPower]"));
	TestTrue(TEXT("Rich text escapes prose"), Rich.Contains(TEXT("A &lt; B &amp;")));
	TestTrue(TEXT("Stat converted to widget tag"), Rich.Contains(TEXT("<coin key=\"STAT:AttackPower\"/>")));
	TestTrue(TEXT("Fixed value stays text"), Rich.Contains(TEXT("10%")));
	TestTrue(TEXT("Unsupported stat is not silently replaced with a fake value"), Rich.Contains(TEXT("[STAT:BossPatternAttackPower]")));
	TestEqual(TEXT("Malformed token stays readable"), FCoinDescriptionFormatter::ToRichText(TEXT("[STAT:Count")), FString(TEXT("[STAT:Count")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCoinDescriptionDatabaseTest, "FlipSide.UI.CoinDescription.Database",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCoinDescriptionDatabaseTest::RunTest(const FString& Parameters)
{
	FSQLiteDatabase Db;
	if (!TestTrue(TEXT("Open production DB read-only"), Db.Open(*(FPaths::ProjectContentDir() / TEXT("DB.db")), ESQLiteDatabaseOpenMode::ReadOnly))) return false;
	FSQLitePreparedStatement Query;
	if (!Query.Create(Db, TEXT("SELECT id,KOR_DES,attack_point,weapon_power,count FROM coin_weapon_def WHERE id>0 ORDER BY id"))) return false;
	int32 WeaponCount = 0;
	while (Query.Step() == ESQLitePreparedStatementStepResult::Row)
	{
		int32 ID = 0;
		FString Raw;
		FWeaponNumericStats Stats;
		Query.GetColumnValueByIndex(0, ID);
		Query.GetColumnValueByIndex(1, Raw);
		Query.GetColumnValueByIndex(2, Stats.AttackPoint);
		Query.GetColumnValueByIndex(3, Stats.WeaponPoint);
		Query.GetColumnValueByIndex(4, Stats.WeaponCnt);
		const auto Data = FCoinDescriptionFormatter::Parse(Raw, CoinDescriptionTestPrivate::Keywords(), Stats);
		TestFalse(FString::Printf(TEXT("Weapon %d contains no retired Miss"), ID), Raw.Contains(TEXT("[KW:Miss]")));
		TestTrue(FString::Printf(TEXT("Weapon %d has 1-3 keyword sections"), ID), Data.Sections.Num() >= 1 && Data.Sections.Num() <= 3);
		for (const auto& Section : Data.Sections)
		{
			TestFalse(TEXT("Weapon header parsed"), Section.MainKeywordCode.IsNone());
			TestEqual(TEXT("DB attack base reaches section"), Section.BaseStats.AttackPoint, Stats.AttackPoint);
		}
		if (ID == 17)
		{
			const auto* Mobility = Data.Sections.FindByPredicate([](const FCoinDescriptionSectionData& Section) { return Section.MainKeywordCode == TEXT("Mobility"); });
			TestTrue(TEXT("Drill includes Absorb"), Mobility && Mobility->AdditionalKeywordCodes.Contains(TEXT("Absorb")));
		}
		++WeaponCount;
	}
	TestTrue(TEXT("Production weapons covered"), WeaponCount >= 18);
	Query.Destroy();
	Db.Close();
	return true;
}
#endif
