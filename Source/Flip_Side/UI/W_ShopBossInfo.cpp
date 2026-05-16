// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopBossInfo.h"
#include "DataTypes/BossDataTypes.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "Subsystem/BossSetupGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/ShopLevel/ShopBossWSubsystem.h"

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

	TArray<UTextBlock*> ButtonTexts = {
		PatternButtonText_1, PatternButtonText_2, PatternButtonText_3,
		PatternButtonText_4, PatternButtonText_5, PatternButtonText_6,
		PatternButtonText_7, PatternButtonText_8, PatternButtonText_9,
		PatternButtonText_10
	};
	for (int32 i = 0; i < ButtonTexts.Num(); i++)
	{
		if (ButtonTexts[i])
			ButtonTexts[i]->SetText(FText::AsNumber(i + 1));
	}

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

	if(!BossSetupGI->HasPreparedBoss())
	{
		BossSetupGI->PrepareBossForID(1);
	}

	FBossDisplayData PreparedBossData;
	if(!BossSetupGI->GetPreparedBossInfo(PreparedBossData))
	{
		ClearBossInfo();
		return;
	}

	UDataManagerSubsystem* DataMgr = GI->GetSubsystem<UDataManagerSubsystem>();

	const int32 StageIndex = BossSetupGI->GetPreparedBossContext().StageIndex;
	CurrentStatMultiplier = 1.0f;
	CurrentGimmickMultiplier = 1.0f;
	if (DataMgr)
		DataMgr->TryGetStageMultiplier(PreparedBossData.BossID, StageIndex, CurrentStatMultiplier, CurrentGimmickMultiplier);

	TArray<FBossPatternDisplayData> PreparedPatternDataList;
	if(DataMgr)
		DataMgr->TryGetBossPatternDisplay(PreparedBossData.BossID, PreparedPatternDataList);

	SetBossInfo(PreparedBossData, PreparedPatternDataList);
}

void UW_ShopBossInfo::SetBossInfo(const FBossDisplayData& BossData)
{
	TArray<FBossPatternDisplayData> EmptyPatternDataList;
	SetBossInfo(BossData, EmptyPatternDataList);
}

void UW_ShopBossInfo::SetBossInfo(const FBossDisplayData& BossData, const TArray<FBossPatternDisplayData>& PatternDataList)
{
	CurrentBossData = BossData;
	CurrentPatternDataList = PatternDataList;
	CurrentPatternIndex = 0;

	if (!IsValid(PatternPreviewActor))
	{
		if (UWorld* World = GetWorld())
		{
			if (UShopBossWSubsystem* Sub = World->GetSubsystem<UShopBossWSubsystem>())
			{
				PatternPreviewActor = Sub->GetPatternPreviewActor();
			}
		}
	}

	if(IsValid(PatternPreviewActor))
	{
		if(CurrentPatternDataList.IsValidIndex(0))
			PatternPreviewActor->ShowPatternPreview(CurrentPatternDataList[0].PatternSpec);
		else
			PatternPreviewActor->ClearPreview();
	}

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

	if(IsValid(PatternPreviewActor))
	{
		PatternPreviewActor->ShowPatternPreview(CurrentPatternDataList[PatternIndex].PatternSpec);
	}

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

	const int32 FinalHP     = static_cast<int32>(CurrentBossData.BossHP * CurrentStatMultiplier);
	const int32 FinalShield = static_cast<int32>(CurrentBossData.ShieldValue * CurrentStatMultiplier);

	if(BossHPText)
	{
		BossHPText->SetText(FText::Format(NSLOCTEXT("BossInfo", "HPFmt", "HP  {0}"), FText::AsNumber(FinalHP)));
		BossHPText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.2f, 0.2f)));
	}

	if(BossShieldText)
	{
		if (FinalShield > 0)
		{
			BossShieldText->SetText(FText::Format(NSLOCTEXT("BossInfo", "ShieldValFmt", "쉴드  {0}"), FText::AsNumber(FinalShield)));
			BossShieldText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 0.6f, 1.f)));
			BossShieldText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			BossShieldText->SetVisibility(ESlateVisibility::Hidden);
		}
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

	const FBossPatternDisplayData& PatternData = CurrentPatternDataList[CurrentPatternIndex];
	const int32 FinalDamage = static_cast<int32>(CurrentBossData.AttackPoint * PatternData.DamageRatio * CurrentStatMultiplier);
	const int32 FinalShieldHeal = static_cast<int32>(PatternData.ShieldHeal * CurrentGimmickMultiplier);
	const int32 FinalGimmickParamA = static_cast<int32>(PatternData.GimmickParamA * CurrentGimmickMultiplier);

	if(PatternRangeImage)
	{
		PatternRangeImage->SetVisibility(ESlateVisibility::HitTestInvisible);
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
		PatternNameText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.85f, 0.3f)));
	}

	if(AttackIconImage)
	{
		AttackIconImage->SetVisibility(FinalDamage > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}

	if(PatternAttackText)
	{
		FText StatText = FText::GetEmpty();

		switch (PatternData.GimmickType)
		{
		case EBossGimmickType::Shield:
			StatText = FText::Format(NSLOCTEXT("BossInfo", "ShieldFmt", "쉴드 회복  {0}"), FText::AsNumber(FinalShieldHeal));
			break;
		case EBossGimmickType::Blind:
			StatText = FText::Format(NSLOCTEXT("BossInfo", "BlindFmt", "실명  {0}턴"), FText::AsNumber(PatternData.GimmickParamA));
			break;
		case EBossGimmickType::GridDebuff:
			StatText = FText::Format(NSLOCTEXT("BossInfo", "SwampFmt", "{0}턴 지속  공격력 -{1}"), FText::AsNumber(PatternData.GimmickParamA), FText::AsNumber(PatternData.GimmickParamB));
			break;
		case EBossGimmickType::Poison:
			StatText = FText::Format(NSLOCTEXT("BossInfo", "PoisonFmt", "공격력  {0}  독 데미지  {1}/5초"), FText::AsNumber(FinalDamage), FText::AsNumber(FinalGimmickParamA));
			break;
		default:
			if (FinalDamage > 0)
				StatText = FText::Format(NSLOCTEXT("BossInfo", "DmgFmt", "공격력  {0}"), FText::AsNumber(FinalDamage));
			break;
		}

		PatternAttackText->SetText(StatText);
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
	CurrentBossData = FBossDisplayData{};
	CurrentPatternIndex = 0;
	CurrentPatternDataList.Reset();

	if(BossImage)
	{
		BossImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if(BossNameText)
		BossNameText->SetText(FText::GetEmpty());

	if(BossHPText)
		BossHPText->SetText(FText::GetEmpty());

	if(BossShieldText)
		BossShieldText->SetVisibility(ESlateVisibility::Hidden);

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

