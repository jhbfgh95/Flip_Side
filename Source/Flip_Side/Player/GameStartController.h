// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameStartController.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AGameStartController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UW_GameStartWidget> GameStartButtonBoxClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UW_SettingWidget> SettingWidgetClass;

	UPROPERTY()
	TObjectPtr<class UW_GameStartWidget> GameStartButtonBox;

	UPROPERTY()
	TObjectPtr<class UW_SettingWidget> SettingWidget;

private:
	UFUNCTION()
	void HandleGameStartRequested();

	UFUNCTION()
	void HandleGameLoadRequested();

	UFUNCTION()
	void HandleGameSettingRequested();

	UFUNCTION()
	void HandleGameTutorialRequested();

	UFUNCTION()
	void HandleGameQuitRequested();

	UFUNCTION()
	void HandleSettingClosed();
};
