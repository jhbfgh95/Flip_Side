// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_GameStartWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStartRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameLoadRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameSettingRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameTutorialRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameQuitRequested);

UCLASS()
class FLIP_SIDE_API UW_GameStartWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// WBP에서 M_IntroVideo 머티리얼을 Brush에 넣어 사용할 전체 화면 Image입니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> IntroVideoImage;

	UPROPERTY(EditDefaultsOnly, Category = "Game Start|Intro")
	TObjectPtr<class UMediaPlayer> IntroMediaPlayer;

	UPROPERTY(EditDefaultsOnly, Category = "Game Start|Intro")
	TObjectPtr<class UMediaSource> IntroMediaSource;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> GameStartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> LoadButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SettingButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TutorialButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> QuitButton;

public:
	UPROPERTY(BlueprintAssignable, Category = "Game Start|Events")
	FOnGameStartRequested OnGameStartRequested;

	UPROPERTY(BlueprintAssignable, Category = "Game Start|Events")
	FOnGameLoadRequested OnGameLoadRequested;

	UPROPERTY(BlueprintAssignable, Category = "Game Start|Events")
	FOnGameSettingRequested OnGameSettingRequested;

	UPROPERTY(BlueprintAssignable, Category = "Game Start|Events")
	FOnGameTutorialRequested OnGameTutorialRequested;

	UPROPERTY(BlueprintAssignable, Category = "Game Start|Events")
	FOnGameQuitRequested OnGameQuitRequested;

	UFUNCTION(BlueprintCallable, Category = "Game Start")
	void CollapseLoadButton();

private:
	UFUNCTION()
	void HandleGameStartButtonClicked();

	UFUNCTION()
	void HandleLoadButtonClicked();

	UFUNCTION()
	void HandleSettingButtonClicked();

	UFUNCTION()
	void HandleTutorialButtonClicked();

	UFUNCTION()
	void HandleQuitButtonClicked();

	UFUNCTION()
	void HandleIntroMediaOpened(FString OpenedUrl);

	UFUNCTION()
	void HandleIntroMediaEndReached();

	void UpdateIntroVideoVisibility();
};
