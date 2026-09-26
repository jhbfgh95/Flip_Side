// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BattleBuffIconWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	const FName SourceIconTextureParameterName(TEXT("SourceIconText"));
	const FName SourceIconColorParameterName(TEXT("SourceIconColor"));
}

void UBattleBuffIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(IconImage))
	{
		BuffIconMaterial = IconImage->GetDynamicMaterial();
		if (!IsValid(BuffIconMaterial))
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[BattleBuffIcon] IconImage Brush에 SourceIconText/SourceIconColor UI 머테리얼을 지정하세요."));
		}
	}
	if (IsValid(SourceImage))
	{
		SourceIconMaterial = SourceImage->GetDynamicMaterial();
		if (!IsValid(SourceIconMaterial))
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[BattleBuffIcon] SourceImage Brush에 SourceIconText/SourceIconColor UI 머테리얼을 지정하세요."));
		}
	}
	ClearBuffData();
}

void UBattleBuffIconWidget::SetBuffData(const FBattleStatusEffectViewData& InData)
{
	if (InData.BuffTypeID == INDEX_NONE)
	{
		ClearBuffData();
		return;
	}

	// 컨트롤러가 전달한 DB 아이콘만 사용하며, 누락 시 아래 이름 표시로 대체합니다.
	UTexture2D* Icon = InData.Icon;
	if (IsValid(RemainingTurnsText))
	{
		RemainingTurnsText->SetText(FText::AsNumber(InData.RemainingTurns));
		RemainingTurnsText->SetVisibility(InData.RemainingTurns > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (IsValid(StatusNameText))
	{
		StatusNameText->SetText(!InData.DisplayName.IsEmpty() ? InData.DisplayName : InData.Polarity == EStatusPolarity::Buff ? NSLOCTEXT("Buff", "Unknown", "버프") :
			InData.CCType == ECCTypes::Blind ? NSLOCTEXT("Debuff", "Blind", "실명") :
			InData.CCType == ECCTypes::Stun ? NSLOCTEXT("Debuff", "Stun", "기절") : NSLOCTEXT("Debuff", "Stat", "스탯 약화"));
		StatusNameText->SetVisibility(!IsValid(Icon) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (IsValid(IconImage)) IconImage->SetVisibility(IsValid(Icon) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	OnStatusDataChanged(InData);
	if (!IsValid(BuffIconMaterial) && IsValid(IconImage))
	{
		BuffIconMaterial = IconImage->GetDynamicMaterial();
	}

	UTexture2D* BorderTexture = nullptr;
	const FLinearColor IconColor = InData.Polarity == EStatusPolarity::Debuff ? DebuffIconColor : BuffIconColor;
	FLinearColor SourceColor = FLinearColor::White;
	const bool bHasSourceStyle = ResolveSourceStyle(InData.SourceType, BorderTexture, SourceColor);
	if (IsValid(SourceImage))
	{
		// WBP에 지정한 공통 머테리얼을 유지하고, 출처별 WBP 텍스처/색상만 전달합니다.
		if (!IsValid(SourceIconMaterial)) SourceIconMaterial = SourceImage->GetDynamicMaterial();
		const bool bShowSource = bHasSourceStyle && IsValid(BorderTexture) && IsValid(SourceIconMaterial);
		if (IsValid(SourceIconMaterial))
		{
			SourceIconMaterial->ClearParameterValues();
			if (bShowSource)
			{
				SourceIconMaterial->SetTextureParameterValue(SourceIconTextureParameterName, BorderTexture);
				SourceIconMaterial->SetVectorParameterValue(SourceIconColorParameterName, SourceColor);
			}
		}
		SourceImage->SetColorAndOpacity(FLinearColor::White);
		SourceImage->SetVisibility(bShowSource ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	if (IsValid(BuffIconMaterial) && IsValid(Icon))
	{
		// 아이콘 전용 머테리얼을 갱신하며 출처 테두리는 SourceImage가 담당합니다.
		BuffIconMaterial->ClearParameterValues();
		BuffIconMaterial->SetTextureParameterValue(SourceIconTextureParameterName, Icon);
		BuffIconMaterial->SetVectorParameterValue(SourceIconColorParameterName, IconColor);
		// 머테리얼 내부에서 아이콘만 착색하므로 Image 전체 틴트는 흰색으로 유지합니다.
		if (IsValid(IconImage)) IconImage->SetColorAndOpacity(FLinearColor::White);
	}
	else if (IsValid(IconImage) && IsValid(Icon))
	{
		// 머테리얼이 빠져도 아이콘 자체는 확인할 수 있도록 안전한 폴백을 둡니다.
		IconImage->SetBrushFromTexture(Icon, false);
		IconImage->SetColorAndOpacity(IconColor);
	}

	const int32 StackCount = FMath::Max(1, InData.StackCount);
	if (IsValid(StackCountText))
	{
		StackCountText->SetText(FText::AsNumber(StackCount));
		StackCountText->SetVisibility(StackCount > 1 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
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
	if (IsValid(SourceImage))
	{
		// Brush를 비우면 WBP 머테리얼까지 사라지므로 파라미터만 초기화합니다.
		if (IsValid(SourceIconMaterial)) SourceIconMaterial->ClearParameterValues();
		SourceImage->SetVisibility(ESlateVisibility::Hidden);
	}
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
