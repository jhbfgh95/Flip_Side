#include "UI/W_BattleCoinInfo.h"

#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UI/BattleBuffIconWidget.h"

namespace
{
	const FName WeaponIconParameterName(TEXT("Weapon_Icon"));
	const FName WeaponColorParameterName(TEXT("Weapon_Color"));
	constexpr int32 FixedBuffIconCount = 3;
}

void UW_BattleCoinInfo::NativeConstruct()
{
	Super::NativeConstruct();

	FrontWeaponMaterial = IsValid(FrontWeaponIcon) ? FrontWeaponIcon->GetDynamicMaterial() : nullptr;
	BackWeaponMaterial = IsValid(BackWeaponIcon) ? BackWeaponIcon->GetDynamicMaterial() : nullptr;
	CacheFixedBuffIcons();
	ClearBattleCoinInfo();
}

void UW_BattleCoinInfo::SetBattleCoinInfo(const FBattleCoinInfoViewData& InData)
{
	SetWeaponFaceData(
		InData.FrontFace,
		FrontWeaponIcon,
		FrontWeaponMaterial,
		FrontWeaponNameText,
		FrontAttackPointText,
		FrontWeaponPointText,
		FrontWeaponCountText,
		FrontWeaponDescriptionText
	);
	SetWeaponFaceData(
		InData.BackFace,
		BackWeaponIcon,
		BackWeaponMaterial,
		BackWeaponNameText,
		BackAttackPointText,
		BackWeaponPointText,
		BackWeaponCountText,
		BackWeaponDescriptionText
	);

	if (IsValid(CoinCurrentHPText))
	{
		CoinCurrentHPText->SetText(FText::AsNumber(FMath::Max(0, InData.CurrentHP)));
	}
	if (IsValid(CoinMaxHPText))
	{
		CoinMaxHPText->SetText(FText::AsNumber(FMath::Max(0, InData.MaxHP)));
	}
	if (IsValid(ShieldText))
	{
		const bool bHasShield = InData.Shield > 0;
		ShieldText->SetText(bHasShield ? FText::AsNumber(InData.Shield) : FText::GetEmpty());
		ShieldText->SetVisibility(
			bHasShield ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	RebuildBuffIcons(InData.StatusEffects);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UW_BattleCoinInfo::ClearBattleCoinInfo()
{
	for (UBattleBuffIconWidget* BuffIcon : FixedBuffIcons)
	{
		if (IsValid(BuffIcon))
		{
			BuffIcon->ClearBuffData();
		}
	}
	if (IsValid(AdditionalBuffPanel))
	{
		AdditionalBuffPanel->ClearChildren();
		AdditionalBuffPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	bHasAdditionalBuffs = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UW_BattleCoinInfo::SetAdditionalBuffsVisible(bool bVisible)
{
	bAdditionalBuffsRequested = bVisible;
	if (IsValid(AdditionalBuffPanel))
	{
		AdditionalBuffPanel->SetVisibility(
			bAdditionalBuffsRequested && bHasAdditionalBuffs
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed
		);
	}
}

void UW_BattleCoinInfo::CacheFixedBuffIcons()
{
	FixedBuffIcons = { BuffIcon1, BuffIcon2, BuffIcon3 };
	for (UBattleBuffIconWidget* BuffIcon : FixedBuffIcons)
	{
		if (IsValid(BuffIcon))
		{
			BuffIcon->ClearBuffData();
		}
	}
}

void UW_BattleCoinInfo::SetWeaponFaceData(
	const FBattleWeaponFaceInfoViewData& FaceData,
	UImage* WeaponIcon,
	TObjectPtr<UMaterialInstanceDynamic>& WeaponMaterial,
	UTextBlock* WeaponNameText,
	URichTextBlock* AttackPointText,
	URichTextBlock* WeaponPointText,
	URichTextBlock* WeaponCountText,
	UTextBlock* WeaponDescriptionText)
{
	if (IsValid(WeaponIcon) && IsValid(FaceData.WeaponIcon))
	{
		if (!IsValid(WeaponMaterial))
		{
			WeaponMaterial = WeaponIcon->GetDynamicMaterial();
		}
		if (IsValid(WeaponMaterial))
		{
			WeaponMaterial->SetTextureParameterValue(WeaponIconParameterName, FaceData.WeaponIcon);
			WeaponMaterial->SetVectorParameterValue(WeaponColorParameterName, FaceData.WeaponColor);
		}
		else
		{
			WeaponIcon->SetBrushFromTexture(FaceData.WeaponIcon);
		}
	}

	if (IsValid(WeaponNameText))
	{
		WeaponNameText->SetText(FaceData.WeaponName);
	}
	if (IsValid(AttackPointText))
	{
		AttackPointText->SetText(FormatStatText(
			TEXT("공격력"), FaceData.BaseStats.AttackPoint, FaceData.FinalStats.AttackPoint));
	}
	if (IsValid(WeaponPointText))
	{
		WeaponPointText->SetText(FormatStatText(
			TEXT("무기력"), FaceData.BaseStats.WeaponPoint, FaceData.FinalStats.WeaponPoint));
	}
	if (IsValid(WeaponCountText))
	{
		WeaponCountText->SetText(FormatStatText(
			TEXT("횟수"), FaceData.BaseStats.WeaponCnt, FaceData.FinalStats.WeaponCnt));
	}
	if (IsValid(WeaponDescriptionText))
	{
		WeaponDescriptionText->SetText(FaceData.WeaponDescription);
	}
}

void UW_BattleCoinInfo::RebuildBuffIcons(const TArray<FBattleStatusEffectViewData>& StatusEffects)
{
	if (FixedBuffIcons.Num() != FixedBuffIconCount)
	{
		CacheFixedBuffIcons();
	}

	for (int32 FixedIndex = 0; FixedIndex < FixedBuffIcons.Num(); ++FixedIndex)
	{
		UBattleBuffIconWidget* BuffIcon = FixedBuffIcons[FixedIndex];
		if (!IsValid(BuffIcon))
		{
			continue;
		}

		if (StatusEffects.IsValidIndex(FixedIndex))
		{
			BuffIcon->SetBuffData(StatusEffects[FixedIndex]);
		}
		else
		{
			BuffIcon->ClearBuffData();
		}
	}

	if (IsValid(AdditionalBuffPanel))
	{
		AdditionalBuffPanel->ClearChildren();
		if (BattleBuffIconWidgetClass)
		{
			for (int32 StatusIndex = FixedBuffIconCount; StatusIndex < StatusEffects.Num(); ++StatusIndex)
			{
				UBattleBuffIconWidget* BuffIcon = CreateWidget<UBattleBuffIconWidget>(
					this, BattleBuffIconWidgetClass);
				if (!IsValid(BuffIcon))
				{
					continue;
				}
				BuffIcon->SetBuffData(StatusEffects[StatusIndex]);
				AdditionalBuffPanel->AddChild(BuffIcon);
			}
		}
	}

	bHasAdditionalBuffs = StatusEffects.Num() > FixedBuffIconCount &&
		IsValid(AdditionalBuffPanel) && AdditionalBuffPanel->GetChildrenCount() > 0;
	SetAdditionalBuffsVisible(bAdditionalBuffsRequested);
}

FText UW_BattleCoinInfo::FormatStatText(const TCHAR* Label, int32 BaseValue, int32 FinalValue)
{
	FString Result = FString::Printf(TEXT("%s %d"), Label, BaseValue);
	const int32 Difference = FinalValue - BaseValue;
	if (Difference > 0)
	{
		Result += FString::Printf(TEXT(" <Green>(+%d)</>"), Difference);
	}
	else if (Difference < 0)
	{
		Result += FString::Printf(TEXT(" <Red>(%d)</>"), Difference);
	}
	return FText::FromString(Result);
}
