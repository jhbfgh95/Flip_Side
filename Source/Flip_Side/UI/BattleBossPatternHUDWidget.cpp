// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BattleBossPatternHUDWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

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
	// TODO: UBossPatternPopupWidget의 표시 구성과 HUD 앵커가 확정되면 Player HUD에서 동적 팝업을 생성합니다.
	OnPatternHovered.Broadcast();
}

void UBattleBossPatternHUDWidget::HandlePatternUnhovered()
{
	OnPatternUnhovered.Broadcast();
}

void UBattleBossPatternHUDWidget::ClearPatternData()
{
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

