// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "W_SettingGraphic.generated.h"

class UCheckBox;
class UComboBoxString;
class UGameSettingGISubsystem;

UCLASS()
class FLIP_SIDE_API UW_SettingGraphic : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ResolutionComboBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> WindowedModeCheckBox;

private:
	UFUNCTION()
	void HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void HandleWindowedModeCheckStateChanged(bool bIsChecked);

	void InitializeResolutionOptions();

	UPROPERTY()
	TObjectPtr<UGameSettingGISubsystem> GameSettingSubsystem;

	TArray<FIntPoint> Resolutions= {
		FIntPoint(1280, 720),
		FIntPoint(1600, 900),
		FIntPoint(1920, 1080),
		FIntPoint(2560, 1440),
		FIntPoint(3840, 2160)
	};
	int32 SelectedResolutionIndex = INDEX_NONE;
	bool bUseWindowedMode = false;

public:
	/** 현재 선택된 해상도를 반환합니다. 선택된 해상도가 없으면 false를 반환합니다. */
	bool GetSelectedResolution(FIntPoint& OutResolution) const;

	/** 현재 선택된 창 모드가 창 모드인지 반환합니다. */
	bool IsWindowedMode() const;
};
