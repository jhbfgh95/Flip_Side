#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_PoisonTimerUI.generated.h"

UCLASS()
class FLIP_SIDE_API UW_PoisonTimerUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* ClockHandImage;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void ShowPoisonTimer(float Duration);

	UFUNCTION(BlueprintCallable)
	void ResetPoisonTimer();

	UFUNCTION(BlueprintCallable)
	void HidePoisonTimer();

private:
	float RemainingTime = 0.f;
	float TotalDuration = 5.f;
	bool bIsActive = false;
};
