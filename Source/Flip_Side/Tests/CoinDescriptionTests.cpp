#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UI/CoinDescriptionFormatter.h"
#include "UI/CoinDescriptionExpression.h"
#include "Misc/Base64.h"
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
	TestEqual(TEXT("Shift is value then icon and name in one bracket"), FCoinDescriptionFormatter::TokenText(Token, true), FString(TEXT("3 [<아이콘> 공격력]")));
	const auto DetailedParts = FCoinDescriptionFormatter::BuildInlineParts(Token, true);
	TestEqual(TEXT("Opening bracket follows value"), DetailedParts.BeforeIcon, FString(TEXT("3 [")));
	TestEqual(TEXT("Closing bracket follows name"), DetailedParts.AfterIcon, FString(TEXT(" 공격력]")));
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
		TestEqual(TEXT("Non numeric name shares icon bracket"), Detail.AfterIcon, TEXT(" ") + Token.Label.ToString() + TEXT("]"));
	}
	FCoinDescriptionFormatter::ResolveToken(TEXT("KW:Attack"), Data, Token);
	const auto BasicKeyword = FCoinDescriptionFormatter::BuildInlineParts(Token, false);
	TestTrue(TEXT("Keywords use the same compact icon-only rule"), BasicKeyword.BeforeIcon.IsEmpty() && BasicKeyword.AfterIcon.IsEmpty());
	TestEqual(TEXT("Keyword detail includes name"), FCoinDescriptionFormatter::BuildInlineParts(Token, true).AfterIcon, FString(TEXT(" Attack]")));
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
		FCoinDescriptionSectionData ExpressionData;
		ExpressionData.BaseStats = Stats;
		int32 Cursor = 0, ExpressionCount = 0;
		while ((Cursor = Raw.Find(TEXT("<Stat>"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Cursor)) != INDEX_NONE)
		{
			const int32 End = Raw.Find(TEXT("</Stat>"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Cursor + 6);
			if (!TestTrue(TEXT("DB formula closes"), End != INDEX_NONE)) break;
			const auto Expression = FCoinDescriptionExpression::Evaluate(Raw.Mid(Cursor + 6, End - Cursor - 6), ExpressionData);
			TestTrue(FString::Printf(TEXT("Weapon %d formula valid: %s"), ID, *Expression.Error), Expression.bValid);
			Cursor = End + 7;
			++ExpressionCount;
		}
		TestTrue(TEXT("Every weapon uses Stat expressions"), ExpressionCount > 0);
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCoinDescriptionExpressionTest, "FlipSide.UI.CoinDescription.Expressions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCoinDescriptionExpressionTest::RunTest(const FString& Parameters)
{
	FCoinDescriptionSectionData Data;
	Data.BaseStats = {3, 7, 2};
	auto Check = [this, &Data](const TCHAR* Text, double Expected)
	{
		const auto Result = FCoinDescriptionExpression::Evaluate(Text, Data);
		TestTrue(Text, Result.bValid && !Result.bHasUnknownValue);
		TestEqual(Text, Result.DisplayValue, Expected);
	};
	Check(TEXT("[STAT:AttackPower] + [STAT:WeaponPower] * 2"), 17.0);
	Check(TEXT("([STAT:AttackPower] + [STAT:WeaponPower]) * 2"), 20.0);
	Check(TEXT("[STAT:AttackPower] / 2 * 3"), 5.0);
	Check(TEXT("[STAT:AttackPower] * 1.2"), 4.0);
	Check(TEXT("[STAT:AttackPower] - 10"), 0.0);
	Check(TEXT("[STAT:WeaponPower] * 20"), 140.0);
	Check(TEXT("[STAT:AttackPower] × [VALUE:2] ÷ 3"), 2.0);
	const auto Duplicate = FCoinDescriptionExpression::Evaluate(TEXT("[STAT:AttackPower] + [STAT:WeaponPower] + [STAT:AttackPower]"), Data);
	TestEqual(TEXT("Unique compact icons"), Duplicate.UniqueKeys.Num(), 2);
	TestEqual(TEXT("Detailed terms preserve repeated stat"), Duplicate.Parts.Num(), 5);
	TestEqual(TEXT("Compact icon order"), Duplicate.UniqueKeys[0], FName(TEXT("STAT:AttackPower")));
	for (const TCHAR* Buff : { TEXT("BUFF:Absorb"), TEXT("BUFF:Strike") })
	{
		const auto Unknown = FCoinDescriptionExpression::Evaluate(FString::Printf(TEXT("[STAT:AttackPower] + [%s]"), Buff), Data);
		TestTrue(TEXT("Buff is unknown, never a computed zero"), Unknown.bValid && Unknown.bHasUnknownValue);
	}
	for (const TCHAR* Invalid : { TEXT("[STAT:AttackPower]/0"), TEXT("[STAT:AttackPower]+"), TEXT("([STAT:Count]"),
		TEXT("[STAT:BossPatternAttackPower]"), TEXT("[STAT:AttackRange]"), TEXT("[STAT:AttackPower]*1.2%"), TEXT("[STAT:Count] 2"), TEXT("2+3") })
		TestFalse(Invalid, FCoinDescriptionExpression::Evaluate(Invalid, Data).bValid);
	TestFalse(TEXT("Depth limit"), FCoinDescriptionExpression::Evaluate(FString::ChrN(40, '(') + TEXT("[STAT:Count]") + FString::ChrN(40, ')'), Data).bValid);
	const FString Formula = TEXT("[STAT:WeaponPower] * 1.2");
	const FString Rich = FCoinDescriptionFormatter::ToRichText(TEXT("확률 <Stat>") + Formula + TEXT("</Stat>% [STAT:Count]"));
	TestTrue(TEXT("Whole formula reaches decorator, percent stays outside"), Rich.Contains(TEXT("<coinexpr expr=\"") + FBase64::Encode(Formula) + TEXT("\"/>%")));
	TestTrue(TEXT("Legacy stat after formula preserved"), Rich.Contains(TEXT("<coin key=\"STAT:Count\"/>")));
	TestEqual(TEXT("Unclosed formula stays visible"), FCoinDescriptionFormatter::ToRichText(TEXT("<Stat>[STAT:Count]")), FString(TEXT("&lt;Stat&gt;[STAT:Count]")));
	const auto Header = FCoinDescriptionFormatter::Parse(TEXT("[KW:Attack] [KW:Continuous] : <Stat>[STAT:WeaponPower]</Stat>\n본문"), {}, Data.BaseStats);
	TestTrue(TEXT("Formula suffix remains a single header"), Header.Sections.Num() == 1 && Header.Sections[0].AdditionalKeywordCodes.Contains(TEXT("Continuous")));
	return true;
}
#endif
