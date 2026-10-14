// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "W_SettingSound.generated.h"

class UEditableTextBox;
class UGameSettingGISubsystem;
class USlider;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingSoundValueChanged, int32, NewValue);

UCLASS()
class FLIP_SIDE_API UW_SettingSound : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> VolumeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> VolumeValueTextBox;

	UPROPERTY(EditDefaultsOnly, Category = "Sound Settings", meta = (ClampMin = "0", ClampMax = "100"))
	int32 MinimumVolume = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Sound Settings", meta = (ClampMin = "0", ClampMax = "100"))
	int32 MaximumVolume = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Sound Settings", meta = (ClampMin = "0", ClampMax = "100"))
	int32 InitialVolume = 100;

public:
	/** 슬라이더 또는 값 입력 UI에서 변경된 정수 볼륨 값입니다. */
	UPROPERTY(BlueprintAssignable, Category = "Sound Settings")
	FOnSettingSoundValueChanged OnSoundValueChanged;

private:
	UFUNCTION()
	void HandleVolumeSliderValueChanged(float NewValue);

	UFUNCTION()
	void HandleVolumeTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	// 슬라이더를 놓았을 때 한 번 저장합니다. 드래그 중에는 적용만 합니다.
	UFUNCTION()
	void HandleVolumeSliderCaptureEnd();

	void SetVolumeValue(int32 NewValue, bool bBroadcastChange);
	int32 GetClampedVolumeValue(int32 Value) const;

	// 슬라이더 값을 게임 전체 볼륨으로 적용·저장합니다.
	UPROPERTY()
	TObjectPtr<UGameSettingGISubsystem> GameSettingSubsystem;

	bool bIsSynchronizingValue = false;
};
