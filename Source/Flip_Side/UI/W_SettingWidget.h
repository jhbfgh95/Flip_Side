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
class UWidgetSwitcher;

UCLASS()
class FLIP_SIDE_API UW_SettingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplySettingButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackgroundButton;

	// 설정 위젯 블루프린트에서 표시할 그래픽 설정 페이지 클래스를 지정한다.
	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	TSubclassOf<UUserWidget> GraphicSettingWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> GraphicSettingWidget;

public:
	UPROPERTY(BlueprintAssignable, Category = "Setting|Events")
	FOnSettingCloseClicked OnCloseClicked;

private:
	UFUNCTION()
	void HandleApplySettingButtonClicked();

	UFUNCTION()
	void HandleCloseButtonClicked();
};
