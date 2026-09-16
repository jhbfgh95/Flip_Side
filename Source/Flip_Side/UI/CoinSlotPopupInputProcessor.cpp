#include "UI/CoinSlotPopupInputProcessor.h"
#include "UI/BattlePlayerHUDWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"

void FCoinSlotPopupInputProcessor::Tick(float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
	if (HUD.IsValid() && HUD->IsCoinSlotInfoOpen() &&
		HUD->GetCoinPopupPointerRegion(SlateApp.GetCursorPos()) == ECoinPopupPointerRegion::OtherUI)
	{
		HUD->DismissCoinSlotInfo();
	}
	// 닫기 판정을 먼저 적용해 다른 UI로 이동한 프레임에 상세 표시가 다시 켜지지 않게 합니다.
	if (HUD.IsValid()) HUD->UpdateCoinDescriptionDetailHover();
}

bool FCoinSlotPopupInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
	if (!HUD.IsValid() || !HUD->IsCoinSlotInfoOpen()) return false;
	const ECoinPopupPointerRegion Region = HUD->GetCoinPopupPointerRegion(Event.GetScreenSpacePosition());
	if (Region == ECoinPopupPointerRegion::OutsideGame) return false;
	const FKey Button = Event.GetEffectingButton();
	if (Button == EKeys::RightMouseButton || (Button == EKeys::LeftMouseButton && Region == ECoinPopupPointerRegion::World))
	{
		HUD->DismissCoinSlotInfo();
		ConsumedButtons.Add(Button);
		return true;
	}
	if (Region == ECoinPopupPointerRegion::OtherUI) HUD->DismissCoinSlotInfo();
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
