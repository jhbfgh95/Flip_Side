#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

class UBattlePlayerHUDWidget;
enum class ECoinPopupPointerRegion : uint8;

/** HUD 생존 기간에만 등록하며, 팝업 닫기 클릭의 press/release를 함께 소비합니다. */
class FCoinSlotPopupInputProcessor : public IInputProcessor
{
public:
	// 코인/아이템의 유지 영역은 다르지만, 닫기 입력 규칙은 동일합니다.
	static bool ShouldDismissOnHover(bool bItemPopup, ECoinPopupPointerRegion Region);
	static bool ShouldConsumeDismissClick(ECoinPopupPointerRegion Region, FKey Button);
	explicit FCoinSlotPopupInputProcessor(UBattlePlayerHUDWidget* InHUD) : HUD(InHUD) {}
	virtual void Tick(float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& Event) override;
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& Event) override;
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& Event) override;
private:
	TWeakObjectPtr<UBattlePlayerHUDWidget> HUD;
	TSet<FKey> ConsumedButtons;
};
