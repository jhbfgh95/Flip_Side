// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_SettingWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingCloseClicked);

/**
 * 
 */

class UButton;
class UOverlay;
class UWidgetSwitcher;

UCLASS()
class FLIP_SIDE_API UW_SettingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackgroundButton;

	/** 해상도 변경 유지 여부를 묻는 오버레이입니다. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ResolutionConfirmOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResolutionConfirmButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResolutionCancelButton;

	/** 해상도 유지 확인 타이머의 남은 시간을 표시합니다. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ResolutionConfirmTimerText;

	// 설정 위젯 블루프린트에서 표시할 그래픽 설정 페이지 클래스를 지정한다.
	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	TSubclassOf<UUserWidget> GraphicSettingWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UW_SettingGraphic> GraphicSettingWidget;
	FTimerHandle ResolutionConfirmTimerHandle;
	FIntPoint PreviousResolution = FIntPoint::ZeroValue;
	bool bHasPendingResolutionChange = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Setting|Events")
	FOnSettingCloseClicked OnCloseClicked;

public:
	void CloseSettingWidget();

private:

	UFUNCTION()
	void HandleResolutionChanged(FIntPoint NewResolution);

	UFUNCTION()
	void HandleResolutionConfirmButtonClicked();

	UFUNCTION()
	void HandleResolutionCancelButtonClicked();

	void RevertPendingResolution();
	void ClearResolutionConfirmationTimer();
	void UpdateResolutionConfirmationTimerText();
	void SetResolutionConfirmOverlayVisible(bool bVisible) const;

	UFUNCTION()
	void HandleCloseButtonClicked();
};
