#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UI/BattlePlayerHUDWidget.h"
#include "UI/CoinSlotPopupInputProcessor.h"
#include "InputCoreTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPersistentPopupInputTest, "FlipSide.UI.PersistentPopup.InputPolicy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FPersistentPopupInputTest::RunTest(const FString& Parameters)
{
	using Region = ECoinPopupPointerRegion;
	for (bool bItem : { false, true })
	{
		const Region Own = bItem ? Region::ItemUI : Region::CoinUI;
		const Region Other = bItem ? Region::CoinUI : Region::ItemUI;
		TestFalse(TEXT("Own popup/slots retain information on hover"), FCoinSlotPopupInputProcessor::ShouldDismissOnHover(bItem, Own));
		TestFalse(TEXT("Leaving UI for world retains information"), FCoinSlotPopupInputProcessor::ShouldDismissOnHover(bItem, Region::World));
		TestFalse(TEXT("Outside game is ignored"), FCoinSlotPopupInputProcessor::ShouldDismissOnHover(bItem, Region::OutsideGame));
		TestTrue(TEXT("Other popup family dismisses"), FCoinSlotPopupInputProcessor::ShouldDismissOnHover(bItem, Other));
		TestTrue(TEXT("Other controls dismiss"), FCoinSlotPopupInputProcessor::ShouldDismissOnHover(bItem, Region::OtherUI));
		TestFalse(TEXT("Own left click remains available for CloseButton or slot use"), FCoinSlotPopupInputProcessor::ShouldConsumeDismissClick(Own, EKeys::LeftMouseButton));
		TestTrue(TEXT("Own right click is dismiss-only"), FCoinSlotPopupInputProcessor::ShouldConsumeDismissClick(Own, EKeys::RightMouseButton));
	}
	TestTrue(TEXT("World left click consumed for closing"), FCoinSlotPopupInputProcessor::ShouldConsumeDismissClick(Region::World, EKeys::LeftMouseButton));
	TestFalse(TEXT("Other UI click allowed after dismissal"), FCoinSlotPopupInputProcessor::ShouldConsumeDismissClick(Region::OtherUI, EKeys::LeftMouseButton));
	TestFalse(TEXT("Outside right click not consumed"), FCoinSlotPopupInputProcessor::ShouldConsumeDismissClick(Region::OutsideGame, EKeys::RightMouseButton));
	return true;
}
#endif
