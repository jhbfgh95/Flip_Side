// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingGame.h"

#include "Components/ComboBoxString.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Kismet/KismetInternationalizationLibrary.h"

void UW_SettingGame::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!IsValid(LanguageComboBox))
	{
		return;
	}

	InitializeLanguageOptions();
	LanguageComboBox->OnSelectionChanged.AddUniqueDynamic(
		this, &ThisClass::HandleLanguageSelectionChanged);
}

void UW_SettingGame::HandleLanguageSelectionChanged(
	FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Direct || !LanguageOptions.Contains(SelectedItem)) return;

#if WITH_EDITOR
	if (GIsEditor)
	{
		FTextLocalizationManager::Get().EnableGameLocalizationPreview(SelectedItem.ToLower());
		return;
	}
#endif

	if (!UKismetInternationalizationLibrary::SetCurrentLanguage(SelectedItem.ToLower(), true))
	{
		const bool bEnglish = UKismetInternationalizationLibrary::GetCurrentLanguage().StartsWith(TEXT("en"));
		LanguageComboBox->SetSelectedIndex(bEnglish ? 1 : 0);
	}
}

void UW_SettingGame::InitializeLanguageOptions()
{
	LanguageComboBox->ClearOptions();

	for (const FString& LanguageOption : LanguageOptions)
	{
		LanguageComboBox->AddOption(LanguageOption);
	}

	bool bEnglish = UKismetInternationalizationLibrary::GetCurrentLanguage().StartsWith(TEXT("en"));
#if WITH_EDITOR
	if (GIsEditor)
	{
		bEnglish = FTextLocalizationManager::Get().IsGameLocalizationPreviewEnabled();
	}
#endif
	LanguageComboBox->SetSelectedIndex(bEnglish ? 1 : 0);
}

