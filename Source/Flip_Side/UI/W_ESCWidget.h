// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ESCWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnESCQuitGameClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnESCContinueGameClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnESCSettingClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnESCMainMenuClicked);

class UButton;
class UOverlay;
class UCanvasPanel;

UCLASS()
class FLIP_SIDE_API UW_ESCWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

	// SettingToggleButton은 이 Canvas Panel 밖에 두어 메뉴가 닫혀도 계속 표시합니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> EscWidget;

	// EscWidget Canvas Panel의 가장 뒤에 배치하는 전체 화면 투명 버튼입니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackgroundCloseButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> MainMenuConfirmOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuConfirmButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuCancelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> QuitConfirmOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitConfirmButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitCancelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingToggleButton;

private:
	UPROPERTY(Transient)
	TObjectPtr<UWidget> CurrentOpenWidget;

public:
	UPROPERTY(BlueprintAssignable, Category = "ESC Menu|Events")
	FOnESCQuitGameClicked OnQuitGameClicked;

	UPROPERTY(BlueprintAssignable, Category = "ESC Menu|Events")
	FOnESCContinueGameClicked OnContinueGameClicked;

	UPROPERTY(BlueprintAssignable, Category = "ESC Menu|Events")
	FOnESCSettingClicked OnSettingClicked;

	UPROPERTY(BlueprintAssignable, Category = "ESC Menu|Events")
	FOnESCMainMenuClicked OnMainMenuClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UW_SettingWidget> SettingWidget;

public:
	bool CloseESCWidget();

	UFUNCTION(BlueprintCallable, Category = "ESC Menu")
	void ToggleESCWidget();

	UFUNCTION(BlueprintPure, Category = "ESC Menu")
	bool IsESCWidgetOpen() const;

private:
	UFUNCTION()
	void HandleQuitGameButtonClicked();

	UFUNCTION()
	void HandleBackgroundCloseButtonClicked();

	UFUNCTION()
	void HandleQuitConfirmButtonClicked();

	UFUNCTION()
	void HandleQuitCancelButtonClicked();

	UFUNCTION()
	void HandleContinueGameButtonClicked();

	UFUNCTION()
	void HandleSettingToggleButtonClicked();

	UFUNCTION()
	void HandleSettingButtonClicked();

	UFUNCTION()
	void HandleMainMenuButtonClicked();

	UFUNCTION()
	void HandleMainMenuConfirmButtonClicked();

	UFUNCTION()
	void HandleMainMenuCancelButtonClicked();

	UFUNCTION()
	void CloseCurrentOpenWidget();

	void OpenWidget(UWidget* WidgetToOpen);

};
