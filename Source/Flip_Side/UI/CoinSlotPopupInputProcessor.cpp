#include "UI/CoinSlotPopupInputProcessor.h"
#include "UI/BattlePlayerHUDWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"

bool FCoinSlotPopupInputProcessor::ShouldDismissOnHover(bool bItemPopup, ECoinPopupPointerRegion Region)
{
	return Region == ECoinPopupPointerRegion::OtherUI ||
		Region == (bItemPopup ? ECoinPopupPointerRegion::CoinUI : ECoinPopupPointerRegion::ItemUI);
}

bool FCoinSlotPopupInputProcessor::ShouldConsumeDismissClick(ECoinPopupPointerRegion Region, FKey Button)
{
	return Region != ECoinPopupPointerRegion::OutsideGame &&
		(Button == EKeys::RightMouseButton || (Button == EKeys::LeftMouseButton && Region == ECoinPopupPointerRegion::World));
}

void FCoinSlotPopupInputProcessor::Tick(float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
	if (!HUD.IsValid() || (!HUD->IsCoinSlotInfoOpen() && !HUD->IsItemInfoOpen())) return;
	const ECoinPopupPointerRegion Region = HUD->GetCoinPopupPointerRegion(SlateApp.GetCursorPos());
	if (HUD->IsCoinSlotInfoOpen() && ShouldDismissOnHover(false, Region))
		HUD->DismissCoinSlotInfo();
	if (HUD->IsItemInfoOpen() && ShouldDismissOnHover(true, Region))
		HUD->DismissItemInfo();
}

bool FCoinSlotPopupInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
	if (!HUD.IsValid() || (!HUD->IsCoinSlotInfoOpen() && !HUD->IsItemInfoOpen())) return false;
	const ECoinPopupPointerRegion Region = HUD->GetCoinPopupPointerRegion(Event.GetScreenSpacePosition());
	if (Region == ECoinPopupPointerRegion::OutsideGame) return false;
	const FKey Button = Event.GetEffectingButton();
	if (ShouldConsumeDismissClick(Region, Button))
	{
		HUD->DismissCoinSlotInfo();
		HUD->DismissItemInfo();
		ConsumedButtons.Add(Button);
		return true;
	}
	if (ShouldDismissOnHover(false, Region)) HUD->DismissCoinSlotInfo();
	if (ShouldDismissOnHover(true, Region)) HUD->DismissItemInfo();
	return false;
}

bool FCoinSlotPopupInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
	return ConsumedButtons.Remove(Event.GetEffectingButton()) > 0;
}

bool FCoinSlotPopupInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
	return HandleMouseButtonDownEvent(SlateApp, Event);
}
