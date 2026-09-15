// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingGame.h"

#include "Components/ComboBoxString.h"

void UW_SettingGame::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!IsValid(LanguageComboBox))
	{
		return;
	}

	LanguageComboBox->OnSelectionChanged.AddUniqueDynamic(
		this, &ThisClass::HandleLanguageSelectionChanged);
	InitializeLanguageOptions();
}

void UW_SettingGame::HandleLanguageSelectionChanged(
	FString /*SelectedItem*/, ESelectInfo::Type /*SelectionType*/)
{
	// 실제 로컬라이제이션 적용은 추후 구현합니다.
}

void UW_SettingGame::InitializeLanguageOptions()
{
	LanguageComboBox->ClearOptions();

	for (const FString& LanguageOption : LanguageOptions)
	{
		LanguageComboBox->AddOption(LanguageOption);
	}

	if (!LanguageOptions.IsEmpty())
	{
		LanguageComboBox->SetSelectedIndex(0);
	}
}

