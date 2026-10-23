#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoinInfoPinHintWidget.generated.h"

/** 코인 호버 중 포인터 오른쪽에 표시하는 입력 안내입니다. */
UCLASS(Abstract)
class FLIP_SIDE_API UCoinInfoPinHintWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdatePositionAtCursor();

protected:
	virtual void NativeOnInitialized() override;

	// 이미지/문구와 크기는 자식 WBP의 Designer에서 설정합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Coin Info Pin Hint")
	FVector2D CursorOffset = FVector2D(24.0f, 0.0f);
};
