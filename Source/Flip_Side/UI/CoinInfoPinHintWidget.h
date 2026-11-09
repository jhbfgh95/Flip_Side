#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "CoinInfoPinHintWidget.generated.h"

class UTexture2D;

enum class ECoinCursorHint : uint8
{
	PinInfo,
	CancelAbility,
	CancelItem
};

/** 코인 정보 고정, 능력 취소 및 아이템 취소를 포인터 옆에 표시합니다. */
UCLASS(Abstract)
class FLIP_SIDE_API UCoinInfoPinHintWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCoinInfoPinHintWidget(const FObjectInitializer& ObjectInitializer);
	bool SetHint(ECoinCursorHint Hint);
	void UpdatePositionAtCursor();

protected:
	virtual void NativeOnInitialized() override;

	// 기본 안내와 공통 크기는 자식 WBP의 Designer에서 설정합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Coin Info Pin Hint")
	FVector2D CursorOffset = FVector2D(24.0f, 0.0f);

private:
	UPROPERTY()
	TObjectPtr<UTexture2D> RightClickIcon;
	UPROPERTY(Transient)
	FSlateBrush PinIconBrush;
	FText PinHintText;
	ECoinCursorHint CurrentHint = ECoinCursorHint::PinInfo;
};
