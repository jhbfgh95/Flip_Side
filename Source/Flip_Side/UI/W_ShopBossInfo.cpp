// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopBossInfo.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "Subsystem/BossSetupGISubsystem.h"

void UW_ShopBossInfo::NativeConstruct()
{
	Super::NativeConstruct();

	CachePatternButtons();

	if(PatternButton_1) PatternButton_1->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern1);
	if(PatternButton_2) PatternButton_2->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern2);
	if(PatternButton_3) PatternButton_3->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern3);
	if(PatternButton_4) PatternButton_4->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern4);
	if(PatternButton_5) PatternButton_5->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern5);
	if(PatternButton_6) PatternButton_6->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern6);
	if(PatternButton_7) PatternButton_7->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern7);
	if(PatternButton_8) PatternButton_8->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern8);
	if(PatternButton_9) PatternButton_9->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern9);
	if(PatternButton_10) PatternButton_10->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::SelectPattern10);

	RefreshPreparedBossInfo();
}

void UW_ShopBossInfo::RefreshPreparedBossInfo()
{
	UGameInstance* GI = GetGameInstance();
	if(!GI)
	{
		ClearBossInfo();
		return;
	}

	UBossSetupGISubsystem* BossSetupGI = GI->GetSubsystem<UBossSetupGISubsystem>();
	if(!BossSetupGI)
	{
		ClearBossInfo();
		return;
	}

	FBossData PreparedBossData;
	TArray<FPatternData> PreparedPatternDataList;
	if(!BossSetupGI->GetPreparedBossInfo(PreparedBossData, PreparedPatternDataList))
	{
		ClearBossInfo();
		return;
	}

	SetBossInfo(PreparedBossData, PreparedPatternDataList);
}

void UW_ShopBossInfo::SetBossInfo(const FBossData& BossData)
{
	TArray<FPatternData> EmptyPatternDataList;
	SetBossInfo(BossData, EmptyPatternDataList);
}

void UW_ShopBossInfo::SetBossInfo(const FBossData& BossData, const TArray<FPatternData>& PatternDataList)
{
	CurrentBossData = BossData;
	CurrentPatternDataList = PatternDataList;
	CurrentPatternIndex = 0;

	SetPatternButtonCount(FMath::Min(CurrentPatternDataList.Num(), PatternButtons.Num()));
	RefreshBossTexts();
	RefreshPatternTexts();
	RefreshPatternButtonState();
}

void UW_ShopBossInfo::SelectPattern(int32 PatternIndex)
{
	if(!PatternButtons.IsValidIndex(PatternIndex))
	{
		return;
	}

	if(!CurrentPatternDataList.IsValidIndex(PatternIndex))
	{
		return;
	}

	CurrentPatternIndex = PatternIndex;

	RefreshPatternTexts();
	RefreshPatternButtonState();
}

void UW_ShopBossInfo::SetPatternButtonCount(int32 PatternCount)
{
	CachePatternButtons();

	for(int32 i = 0; i < PatternButtons.Num(); i++)
	{
		if(!PatternButtons[i]) continue;

		const bool bShouldShow = i < PatternCount;
		PatternButtons[i]->SetVisibility(bShouldShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if(PatternCount <= 0)
	{
		CurrentPatternIndex = 0;
	}
	else
	{
		CurrentPatternIndex = FMath::Clamp(CurrentPatternIndex, 0, PatternCount - 1);
	}

	RefreshPatternButtonState();
}

void UW_ShopBossInfo::SelectPattern1()
{
	SelectPattern(0);
}

void UW_ShopBossInfo::SelectPattern2()
{
	SelectPattern(1);
}

void UW_ShopBossInfo::SelectPattern3()
{
	SelectPattern(2);
}

void UW_ShopBossInfo::SelectPattern4()
{
	SelectPattern(3);
}

void UW_ShopBossInfo::SelectPattern5()
{
	SelectPattern(4);
}

void UW_ShopBossInfo::SelectPattern6()
{
	SelectPattern(5);
}

void UW_ShopBossInfo::SelectPattern7()
{
	SelectPattern(6);
}

void UW_ShopBossInfo::SelectPattern8()
{
	SelectPattern(7);
}

void UW_ShopBossInfo::SelectPattern9()
{
	SelectPattern(8);
}

void UW_ShopBossInfo::SelectPattern10()
{
	SelectPattern(9);
}

void UW_ShopBossInfo::CachePatternButtons()
{
	PatternButtons.Reset();
	PatternButtons.Add(PatternButton_1);
	PatternButtons.Add(PatternButton_2);
	PatternButtons.Add(PatternButton_3);
	PatternButtons.Add(PatternButton_4);
	PatternButtons.Add(PatternButton_5);
	PatternButtons.Add(PatternButton_6);
	PatternButtons.Add(PatternButton_7);
	PatternButtons.Add(PatternButton_8);
	PatternButtons.Add(PatternButton_9);
	PatternButtons.Add(PatternButton_10);
}

void UW_ShopBossInfo::RefreshBossTexts()
{
	if(BossImage)
	{
		if(CurrentBossData.BossImage)
		{
			BossImage->SetBrushFromTexture(CurrentBossData.BossImage);
			BossImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			BossImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(BossNameText)
	{
		BossNameText->SetText(FText::FromString(CurrentBossData.BossName));
	}

	if(BossAbilityText)
	{
		BossAbilityText->SetText(CurrentBossData.BossAbilityDescription);
	}
}

void UW_ShopBossInfo::RefreshPatternTexts()
{
	if(PatternTitleText)
	{
		PatternTitleText->SetText(FText::FromString(TEXT("패턴")));
	}

	if(!CurrentPatternDataList.IsValidIndex(CurrentPatternIndex))
	{
		ClearPatternInfo();
		return;
	}

	const FPatternData& PatternData = CurrentPatternDataList[CurrentPatternIndex];
	const int32 FinalDamage = CurrentBossData.AttackPoint + PatternData.Damage;

	if(PatternRangeImage)
	{
		if(PatternData.PatternRangeImage)
		{
			PatternRangeImage->SetBrushFromTexture(PatternData.PatternRangeImage);
			PatternRangeImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PatternRangeImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(PatternIconImage)
	{
		if(PatternData.PatternIcon)
		{
			PatternIconImage->SetBrushFromTexture(PatternData.PatternIcon);
			PatternIconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PatternIconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(PatternNameText)
	{
		PatternNameText->SetText(FText::FromString(PatternData.PatternName));
	}

	if(PatternAttackText)
	{
		PatternAttackText->SetText(FText::AsNumber(FinalDamage));
	}

	if(PatternDescriptionText)
	{
		PatternDescriptionText->SetText(PatternData.PatternDescription);
	}
}

void UW_ShopBossInfo::RefreshPatternButtonState()
{
	for(int32 i = 0; i < PatternButtons.Num(); i++)
	{
		if(!PatternButtons[i]) continue;

		PatternButtons[i]->SetIsEnabled(i != CurrentPatternIndex);
	}
}

void UW_ShopBossInfo::ClearBossInfo()
{
	CurrentBossData = FBossData{};
	CurrentPatternIndex = 0;
	CurrentPatternDataList.Reset();

	if(BossImage)
	{
		BossImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if(BossNameText)
	{
		BossNameText->SetText(FText::GetEmpty());
	}

	if(BossAbilityText)
	{
		BossAbilityText->SetText(FText::GetEmpty());
	}

	SetPatternButtonCount(0);
	ClearPatternInfo();
}

void UW_ShopBossInfo::ClearPatternInfo()
{
	if(PatternRangeImage)
	{
		PatternRangeImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if(PatternIconImage)
	{
		PatternIconImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if(PatternNameText)
	{
		PatternNameText->SetText(FText::GetEmpty());
	}

	if(PatternAttackText)
	{
		PatternAttackText->SetText(FText::GetEmpty());
	}

	if(PatternDescriptionText)
	{
		PatternDescriptionText->SetText(FText::GetEmpty());
	}
}

