// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BossPatternPopupWidget.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"

void UBossPatternPopupWidget::SetPatternInfo(const FBossHUDData& InData)
{
	if (IsValid(PatternNameText)) PatternNameText->SetText(InData.bHasPatternInfo
		? FText::FromString(InData.PatternName) : FText::GetEmpty());
	// DB 설명을 그대로 전달합니다. RichText 스타일 테이블/Decorator는 팝업 BP에서 지정합니다.
	if (IsValid(PatternDescriptionText)) PatternDescriptionText->SetText(InData.bHasPatternInfo
		? InData.PatternDescription : FText::GetEmpty());
	auto SetStat = [](UTextBlock* Text, UWidget* Container, bool bVisible, const FText& Value)
	{
		const ESlateVisibility Visibility = bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
		if (IsValid(Text)) { Text->SetText(bVisible ? Value : FText::GetEmpty()); Text->SetVisibility(Visibility); }
		if (IsValid(Container)) Container->SetVisibility(Visibility);
	};
	SetStat(AttackPointText, AttackPointContainer, InData.bHasPatternInfo && InData.bShowPatternDamage,
		InData.bPatternNoDamage ? NSLOCTEXT("BossPatternPopup", "NoDamage", "피해 없음") : FText::AsNumber(InData.PatternDamage));
	SetStat(ConditionalAttackPointText, ConditionalAttackPointContainer,
		InData.bHasPatternInfo && InData.bShowPatternDamage && !InData.bPatternNoDamage && InData.bHasConditionalPatternDamage,
		FText::AsNumber(InData.ConditionalPatternDamage));
	SetStat(ShieldHealText, ShieldHealContainer, InData.bHasPatternInfo && InData.bShowPatternShieldHeal,
		FText::AsNumber(InData.PatternShieldHeal));
	SetVisibility(InData.bHasPatternInfo ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

