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
	UFUNCTION()
	void HandleLanguageSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void InitializeLanguageOptions();

	UPROPERTY()
	TArray<FString> LanguageOptions = { TEXT("Ko"), TEXT("En") };
};
