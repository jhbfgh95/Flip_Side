// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BattleBossPatternHUDWidget.h"

#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"

void UBattleBossPatternHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(PatternButton))
	{
		PatternButton->OnHovered.RemoveAll(this);
		PatternButton->OnUnhovered.RemoveAll(this);
		PatternButton->OnHovered.AddDynamic(this, &UBattleBossPatternHUDWidget::HandlePatternHovered);
		PatternButton->OnUnhovered.AddDynamic(this, &UBattleBossPatternHUDWidget::HandlePatternUnhovered);
	}
}

void UBattleBossPatternHUDWidget::SetBossHUDData(const FBossHUDData& InData)
{
	if (!InData.bHasPatternInfo)
	{
		ClearPatternData();
		return;
	}

	if (IsValid(PatternName))
	{
		PatternName->SetText(FText::FromString(InData.PatternName));
	}

	if (IsValid(PatternTypeText))
	{
		PatternTypeText->SetText(InData.bIsGimmick
			? NSLOCTEXT("BattleBossPatternHUD", "GimmickType", "<Gimmick>기믹</>")
			: NSLOCTEXT("BattleBossPatternHUD", "PatternType", "<Pattern>패턴</>"));
	}

	if (IsValid(PatternTypeBorder))
	{
		PatternTypeBorder->SetBrushColor(InData.bIsGimmick
			? GimmickBackgroundColor : PatternBackgroundColor);
	}

	if (IsValid(PatternDamage))
	{
		PatternDamage->SetText(InData.bShowPatternDamage
			? (InData.bPatternNoDamage
				? NSLOCTEXT("BattleBossPatternHUD", "NoDamage", "피해 없음")
				: FText::AsNumber(InData.PatternDamage))
			: FText::GetEmpty());
		PatternDamage->SetVisibility(InData.bShowPatternDamage
			? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	const bool bShowConditionalDamage = InData.bShowPatternDamage
		&& !InData.bPatternNoDamage && InData.bHasConditionalPatternDamage;
	if (IsValid(ConditonalPatternDamage))
	{
		ConditonalPatternDamage->SetText(bShowConditionalDamage
			? FText::AsNumber(InData.ConditionalPatternDamage) : FText::GetEmpty());
	}
	if (IsValid(ConditionalPatternDamagePannel))
	{
		ConditionalPatternDamagePannel->SetVisibility(bShowConditionalDamage
			? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (IsValid(PatternImage))
	{
		if (IsValid(InData.PatternIcon))
		{
			PatternImage->SetBrushFromTexture(InData.PatternIcon);
			PatternImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PatternImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	SetVisibility(ESlateVisibility::Visible);
}

void UBattleBossPatternHUDWidget::HandlePatternHovered()
{
	// HUD가 고정 앵커 위치에 BossPatternPopupWidget을 생성하고 최신 표시 데이터를 전달합니다.
	OnPatternHovered.Broadcast();
}

void UBattleBossPatternHUDWidget::HandlePatternUnhovered()
{
	OnPatternUnhovered.Broadcast();
}

void UBattleBossPatternHUDWidget::ClearPatternData()
{
	if (IsValid(PatternTypeText))
	{
		PatternTypeText->SetText(FText::GetEmpty());
	}

	if (IsValid(PatternDamage))
	{
		PatternDamage->SetText(FText::GetEmpty());
		PatternDamage->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (IsValid(ConditonalPatternDamage))
	{
		ConditonalPatternDamage->SetText(FText::GetEmpty());
	}
	if (IsValid(ConditionalPatternDamagePannel))
	{
		ConditionalPatternDamagePannel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsValid(PatternName))
	{
		PatternName->SetText(FText::GetEmpty());
	}

	if (IsValid(PatternImage))
	{
		PatternImage->SetVisibility(ESlateVisibility::Hidden);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

