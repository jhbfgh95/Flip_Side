// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "W_SettingGraphic.generated.h"

class UComboBoxString;
class UGameSettingGISubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResolutionChanged, FIntPoint, NewResolution);

UCLASS()
class FLIP_SIDE_API UW_SettingGraphic : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ResolutionComboBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> WindowModeComboBox;

private:
	UFUNCTION()
	void HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void HandleWindowModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void InitializeResolutionOptions();
	void InitializeWindowModeOptions();
	void RefreshWindowModeOptions();

	UPROPERTY()
	TObjectPtr<UGameSettingGISubsystem> GameSettingSubsystem;

	TArray<FIntPoint> Resolutions= {
		FIntPoint(1280, 720),
		FIntPoint(1920, 1080)
	};
	int32 SelectedResolutionIndex = INDEX_NONE;
	EWindowMode::Type SelectedWindowMode = EWindowMode::Fullscreen;
	bool bIsSynchronizingResolutionSelection = false;
	bool bIsRefreshingWindowModeOptions = false;

public:
	/** 해상도 선택이 변경될 때 선택된 해상도를 알립니다. */
	UPROPERTY(BlueprintAssignable, Category = "Setting|Events")
	FOnResolutionChanged OnResolutionChanged;

	/** 현재 선택된 해상도를 반환합니다. 선택된 해상도가 없으면 false를 반환합니다. */
	bool GetSelectedResolution(FIntPoint& OutResolution) const;

	/** 콤보박스의 선택 해상도를 변경합니다. 델리게이트는 호출하지 않습니다. */
	void SetSelectedResolution(FIntPoint Resolution);

	/** 현재 선택된 창 모드를 반환합니다. */
	EWindowMode::Type GetSelectedWindowMode() const;
};
