#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "SQLiteDatabase.h"
#include "SQLitePreparedStatement.h"
#include "UI/ItemDescriptionFormatter.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FItemDescriptionFormattingTest, "FlipSide.UI.ItemDescription.Formatting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FItemDescriptionFormattingTest::RunTest(const FString& Parameters)
{
	const auto Data = FItemDescriptionFormatter::Format(TEXT("대상 : 코인\\n효과 : [공격]으로 회복한다.\\n[적중]"));
	TestEqual(TEXT("Only standalone keyword becomes header"), Data.Header, FString(TEXT("<itemkw code=\"Hit\"/>")));
	TestEqual(TEXT("Border color key ignores inline Attack"), Data.HeaderKeyword, FName(TEXT("Hit")));
	TestTrue(TEXT("No header clears Border key"), FItemDescriptionFormatter::Format(TEXT("[공격]으로 피해")).HeaderKeyword.IsNone());
	TestTrue(TEXT("Attack remains inline"), Data.Body.Contains(TEXT("<itemkw code=\"Attack\"/>으로")));
	TestTrue(TEXT("Legacy labels stay plain without table styles"), Data.Body.StartsWith(TEXT("대상 : 코인\n효과 : ")));
	TestFalse(TEXT("Header removed from body"), Data.Body.Contains(TEXT("Hit")));
	TestFalse(TEXT("Legacy escapes normalized"), Data.PlainText.Contains(TEXT("\\n")));
	const auto Escaped = FItemDescriptionFormatter::Format(TEXT("효과 : A < B & [없는키워드]\n[즉시]"));
	TestTrue(TEXT("Prose escaped without interpreting markup"), Escaped.Body.Contains(TEXT("A &lt; B &amp; [없는키워드]")));
	TestEqual(TEXT("Instant supported"), Escaped.Header, FString(TEXT("<itemkw code=\"Instant\"/>")));
	const auto Lines = FItemDescriptionFormatter::Format(TEXT("대상 : 코인\r\n효과 : 설치\r\n다음 턴 파괴\r\n[즉시]"));
	TestTrue(TEXT("Continuation newline retained"), Lines.Body.EndsWith(TEXT("설치\n다음 턴 파괴")));
	TestTrue(TEXT("Empty clears everything"), FItemDescriptionFormatter::Format(FString()).Body.IsEmpty());
	const auto Indented = FItemDescriptionFormatter::Format(TEXT("\t첫 줄\n\t둘째 줄\n\t[즉시]"));
	TestEqual(TEXT("Document indentation preserved in body"), Indented.Body, FString(TEXT("\t첫 줄\n\t둘째 줄")));
	TestEqual(TEXT("Double escaped legacy newline"), FItemDescriptionFormatter::Format(TEXT("A\\\\nB")).PlainText, FString(TEXT("A\nB")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FItemDescriptionDatabaseTest, "FlipSide.UI.ItemDescription.Database",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FItemDescriptionDatabaseTest::RunTest(const FString& Parameters)
{
	FSQLiteDatabase Db;
	if (!TestTrue(TEXT("Open readonly DB"), Db.Open(*(FPaths::ProjectContentDir() / TEXT("DB.db")), ESQLiteDatabaseOpenMode::ReadOnly))) return false;
	FSQLitePreparedStatement Query;
	if (!TestTrue(TEXT("Prepare items"), Query.Create(Db, TEXT("SELECT item_id,item_description FROM item WHERE item_id BETWEEN 1 AND 6 ORDER BY item_id")))) return false;
	int32 Count = 0;
	while (Query.Step() == ESQLitePreparedStatementStepResult::Row)
	{
		int32 ID;
		FString Raw;
		Query.GetColumnValueByIndex(0, ID);
		Query.GetColumnValueByIndex(1, Raw);
		const auto Display = FItemDescriptionFormatter::Format(Raw);
		TestFalse(TEXT("Shared DB has no RichText markup"), Raw.Contains(TEXT("<")));
		TestTrue(TEXT("Actual newline works in shop TextBlock"), Raw.Contains(TEXT("\n")) && !Raw.Contains(TEXT("\\n")));
		TestFalse(TEXT("No target title"), Raw.Contains(TEXT("대상 :")));
		TestFalse(TEXT("No effect title"), Raw.Contains(TEXT("효과 :")));
		TestFalse(TEXT("No title table tags"), Display.Body.Contains(TEXT("<Item")));
		TestEqual(TEXT("Expected header only"), Display.Header, FString(ID <= 2 ? TEXT("<itemkw code=\"Hit\"/>") : TEXT("<itemkw code=\"Instant\"/>")));
		if (ID == 3) TestTrue(TEXT("Cleanse keeps document tab indentation"), Display.Body.StartsWith(TEXT("\t필드")) && Display.Body.Contains(TEXT("\n\t선택")));
		++Count;
	}
	TestEqual(TEXT("All six items covered"), Count, 6);
	Query.Destroy();
	if (!TestTrue(TEXT("Prepare Instant"), Query.Create(Db, TEXT("SELECT description_ko FROM keyword_definition WHERE keyword_code='Instant' AND is_enabled=1")))) return false;
	if (TestTrue(TEXT("Instant exists"), Query.Step() == ESQLitePreparedStatementStepResult::Row))
	{
		FString Description;
		Query.GetColumnValueByIndex(0, Description);
		TestEqual(TEXT("Approved Instant definition"), Description, FString(TEXT("아이템을 사용하면, 해당 시점에 바로 나타나는 효과입니다.")));
	}
	Query.Destroy();
	Db.Close();
	return true;
}
#endif
