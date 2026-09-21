// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopBossPatternButton.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UW_ShopBossPatternButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PatternButton)
	{
		PatternButton->OnClicked.AddUniqueDynamic(this, &UW_ShopBossPatternButton::HandlePatternButtonClicked);
	}
}

void UW_ShopBossPatternButton::InitPatternButton(int32 SetButtonIndex, const FText& InButtonText)
{
	ButtonIndex = SetButtonIndex;

	if (PatternButtonText)
	{
		PatternButtonText->SetText(InButtonText);
	}
}

void UW_ShopBossPatternButton::HandlePatternButtonClicked()
{
	OnShopBossPatternButtonClicked.Broadcast(ButtonIndex);
}
