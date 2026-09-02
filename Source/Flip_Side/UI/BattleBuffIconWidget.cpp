// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BattleBuffIconWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	const FName BuffIconParameterName(TEXT("Buff_Icon"));
	const FName BuffSourceBorderParameterName(TEXT("Buff_SourceBorder"));
	const FName BuffSourceColorParameterName(TEXT("Buff_SourceColor"));
}

void UBattleBuffIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(BuffIconImage))
	{
		BuffIconMaterial = BuffIconImage->GetDynamicMaterial();
		if (!IsValid(BuffIconMaterial))
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[BattleBuffIcon] BuffIconImage Brush에 상태효과 UI 머테리얼을 지정하세요."));
		}
	}
	ClearBuffData();
}

void UBattleBuffIconWidget::SetBuffData(const FBattleStatusEffectViewData& InData)
{
	if (InData.BuffTypeID == INDEX_NONE || !IsValid(InData.Icon) || !IsValid(BuffIconImage))
	{
		ClearBuffData();
		return;
	}

	if (!IsValid(BuffIconMaterial))
	{
		BuffIconMaterial = BuffIconImage->GetDynamicMaterial();
	}

	UTexture2D* BorderTexture = nullptr;
	FLinearColor SourceColor = FLinearColor::White;
	const bool bHasSourceStyle = ResolveSourceStyle(InData.SourceType, BorderTexture, SourceColor);
	if (IsValid(BuffIconMaterial))
	{
		BuffIconMaterial->SetTextureParameterValue(BuffIconParameterName, InData.Icon);
		if (bHasSourceStyle && IsValid(BorderTexture))
		{
			BuffIconMaterial->SetTextureParameterValue(BuffSourceBorderParameterName, BorderTexture);
		}
		BuffIconMaterial->SetVectorParameterValue(BuffSourceColorParameterName, SourceColor);
	}
	else
	{
		// 머테리얼이 빠져도 아이콘 자체는 확인할 수 있도록 안전한 폴백을 둡니다.
		BuffIconImage->SetBrushFromTexture(InData.Icon);
	}

	const int32 StackCount = FMath::Max(1, InData.StackCount);
	if (IsValid(StackCountText))
	{
		StackCountText->SetText(FText::AsNumber(StackCount));
	}
	if (IsValid(StackCountContainer))
	{
		StackCountContainer->SetVisibility(
			StackCount > 1 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	else if (IsValid(StackCountText))
	{
		StackCountText->SetVisibility(
			StackCount > 1 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UBattleBuffIconWidget::ClearBuffData()
{
	if (IsValid(StackCountText))
	{
		StackCountText->SetText(FText::GetEmpty());
		StackCountText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (IsValid(StackCountContainer))
	{
		StackCountContainer->SetVisibility(ESlateVisibility::Collapsed);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

bool UBattleBuffIconWidget::ResolveSourceStyle(
	EStatusEffectSourceType SourceType,
	UTexture2D*& OutBorderTexture,
	FLinearColor& OutSourceColor) const
{
	OutBorderTexture = nullptr;
	OutSourceColor = FLinearColor::White;

	switch (SourceType)
	{
	case EStatusEffectSourceType::Coin:
		OutBorderTexture = CoinBorderTexture;
		OutSourceColor = CoinSourceColor;
		return true;
	case EStatusEffectSourceType::Item:
		OutBorderTexture = ItemBorderTexture;
		OutSourceColor = ItemSourceColor;
		return true;
	case EStatusEffectSourceType::Boss:
		// TODO: Boss 주체 버프가 실제로 추가되면 보스용 아이콘 조회 결과와 이 스타일을 활성 사용합니다.
		OutBorderTexture = BossBorderTexture;
		OutSourceColor = BossSourceColor;
		return true;
	default:
		return false;
	}
}
