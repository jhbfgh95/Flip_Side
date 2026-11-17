// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_SettingGame.h"

#include "Components/ComboBoxString.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Internationalization/TextLocalizationResource.h"
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
		// 에디터 실행 중 처음 번역을 컴파일하면 시작 시 캐시된 기본 언어가 비어 있을 수 있습니다.
		// 기본 언어로 복귀할 때도 최신 locmeta를 사용해 한국어 locres를 다시 로드합니다.
		TextLocalizationResourceUtil::ClearNativeProjectCultureName();
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

