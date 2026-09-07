// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "W_SettingGame.generated.h"

class UComboBoxString;

UCLASS()
class FLIP_SIDE_API UW_SettingGame : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> LanguageComboBox;

private:
	/** 언어 선택 UI 이벤트입니다. 실제 언어 변경은 추후 구현합니다. */
	UFUNCTION()
	void HandleLanguageSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void InitializeLanguageOptions();

	UPROPERTY()
	TArray<FString> LanguageOptions = { TEXT("Ko"), TEXT("En") };
};
