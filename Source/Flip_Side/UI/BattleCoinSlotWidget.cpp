// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BattleCoinSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace BattleCoinSlotWidgetPrivate
{
	const FName WeaponIconParameterName(TEXT("Weapon_Icon"));
	const FName WeaponColorParameterName(TEXT("Weapon_Color"));
	const FLinearColor FrontWeaponColor(0.862745f, 0.913725f, 0.313725f, 1.0f);
	const FLinearColor BackWeaponColor(0.905882f, 0.933333f, 0.917647f, 1.0f);
}

void UBattleCoinSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(CoinSlotButton))
	{
		CoinSlotButton->OnClicked.AddDynamic(this, &UBattleCoinSlotWidget::HandleCoinButtonClicked);
		CoinSlotButton->OnHovered.AddDynamic(this, &UBattleCoinSlotWidget::HandleCoinButtonHovered);
		CoinSlotButton->OnUnhovered.AddDynamic(this, &UBattleCoinSlotWidget::HandleCoinButtonUnhovered);
	}
}

void UBattleCoinSlotWidget::SetSlotData(const FBattleCoinSlotViewData& InData)
{
	SlotNumber = InData.SlotNumber;

	if (IsValid(SlotNumberText))
	{
		SlotNumberText->SetText(FText::AsNumber(InData.SlotNumber));
	}

	if (IsValid(CoinCountText))
	{
		CoinCountText->SetText(FText::AsNumber(InData.CoinCount));
	}

	if (IsValid(HPText))
	{
		HPText->SetText(FText::AsNumber(InData.HP));
	}

	if (IsValid(FrontWeaponIcon))
	{
		UpdateWeaponIconMaterial(
			FrontWeaponIcon,
			InData.FrontIcon,
			FrontWeaponIconMaterialInstance,
			BattleCoinSlotWidgetPrivate::FrontWeaponColor
		);
	}

	if (IsValid(BackWeaponIcon))
	{
		UpdateWeaponIconMaterial(
			BackWeaponIcon,
			InData.BackIcon,
			BackWeaponIconMaterialInstance,
			BattleCoinSlotWidgetPrivate::BackWeaponColor
		);
	}

	// TODO: 코인 아이콘 Dynamic Material과 타입 색상 처리는 머티리얼 준비 후 추가합니다.
	if (IsValid(CoinSlotButton))
	{
		CoinSlotButton->SetIsEnabled(InData.CoinCount > 0);
	}
	SetVisibility(ESlateVisibility::Visible);
}

void UBattleCoinSlotWidget::ClearSlotData()
{
	SlotNumber = INDEX_NONE;
	if (IsValid(CoinSlotButton))
	{
		CoinSlotButton->SetIsEnabled(false);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void UBattleCoinSlotWidget::HandleCoinButtonClicked()
{
	if (SlotNumber != INDEX_NONE)
	{
		OnBattleCoinSlotClicked.Broadcast(SlotNumber);
	}
}

void UBattleCoinSlotWidget::HandleCoinButtonHovered()
{
	if (SlotNumber != INDEX_NONE)
	{
		OnBattleCoinSlotHovered.Broadcast(SlotNumber);
	}
}

void UBattleCoinSlotWidget::HandleCoinButtonUnhovered()
{
	if (SlotNumber != INDEX_NONE)
	{
		OnBattleCoinSlotUnhovered.Broadcast(SlotNumber);
	}
}

void UBattleCoinSlotWidget::UpdateWeaponIconMaterial(
	UImage* WeaponIconImage,
	UTexture2D* WeaponIconTexture,
	TObjectPtr<UMaterialInstanceDynamic>& DynamicMaterial,
	const FLinearColor& WeaponColor
)
{
	if (!IsValid(WeaponIconImage) || !IsValid(WeaponIconTexture))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BattleCoinSlot] Weapon material update skipped. Image=%s, Texture=%s"),
			*GetNameSafe(WeaponIconImage),
			*GetNameSafe(WeaponIconTexture)
		);
		return;
	}

	// 각 WeaponIcon Image의 BP Brush에 지정된 머티리얼에서 MID를 가져옵니다.
	// Brush를 코드에서 교체하지 않으므로, BP에서 설정한 UI 머티리얼을 그대로 사용합니다.
	DynamicMaterial = WeaponIconImage->GetDynamicMaterial();
	if (!IsValid(DynamicMaterial))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BattleCoinSlot] GetDynamicMaterial failed. %s Brush에 UI 머티리얼이 지정되어 있는지 확인하세요."),
			*GetNameSafe(WeaponIconImage)
		);
		return;
	}

	DynamicMaterial->SetTextureParameterValue(
		BattleCoinSlotWidgetPrivate::WeaponIconParameterName, WeaponIconTexture);
	DynamicMaterial->SetVectorParameterValue(
		BattleCoinSlotWidgetPrivate::WeaponColorParameterName, WeaponColor);
}


