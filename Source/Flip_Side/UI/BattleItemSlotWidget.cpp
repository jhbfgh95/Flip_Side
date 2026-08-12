// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BattleItemSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	const FName ItemIconParameterName(TEXT("Weapon_Icon"));
	const FName ItemColorParameterName(TEXT("Weapon_Color"));
}

void UBattleItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ItemButton))
	{
		ItemButton->OnClicked.AddDynamic(this, &UBattleItemSlotWidget::HandleItemButtonClicked);
		ItemButton->OnHovered.AddDynamic(this, &UBattleItemSlotWidget::HandleItemButtonHovered);
		ItemButton->OnUnhovered.AddDynamic(this, &UBattleItemSlotWidget::HandleItemButtonUnhovered);
	}
}

void UBattleItemSlotWidget::SetItemData(const FBattleItemSlotViewData& InData)
{
	ItemID = InData.ItemData.ItemID;
	AvailableCount = InData.AvailableCount;
	bCanUse = InData.bCanUse;

	if (IsValid(ItemCountText))
	{
		ItemCountText->SetText(FText::AsNumber(AvailableCount));
	}

	if (IsValid(ItemNameText))
	{
		ItemNameText->SetText(FText::FromString(InData.ItemData.ItemName));
	}

	if (IsValid(ItemIcon) && IsValid(InData.ItemData.ItemIcon))
	{
		ItemIconMaterialInstance = ItemIcon->GetDynamicMaterial();
		if (IsValid(ItemIconMaterialInstance))
		{
			ItemIconMaterialInstance->SetTextureParameterValue(ItemIconParameterName, InData.ItemData.ItemIcon);
			ItemIconMaterialInstance->SetVectorParameterValue(ItemColorParameterName, InData.ItemData.TypeColor);
		}
		else
		{
			ItemIcon->SetBrushFromTexture(InData.ItemData.ItemIcon, false);
		}
	}

	// ReadyPhase에서도 호버 정보가 필요하므로 버튼은 비활성화하지 않습니다.
	SetVisibility(AvailableCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UBattleItemSlotWidget::ClearItemData()
{
	ItemID = INDEX_NONE;
	AvailableCount = 0;
	bCanUse = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UBattleItemSlotWidget::HandleItemButtonClicked()
{
	if (ItemID != INDEX_NONE && AvailableCount > 0 && bCanUse)
	{
		OnBattleItemSlotClicked.Broadcast(ItemID);
	}
}

void UBattleItemSlotWidget::HandleItemButtonHovered()
{
	if (ItemID != INDEX_NONE && AvailableCount > 0)
	{
		OnBattleItemSlotHovered.Broadcast(ItemID);
	}
}

void UBattleItemSlotWidget::HandleItemButtonUnhovered()
{
	if (ItemID != INDEX_NONE)
	{
		OnBattleItemSlotUnhovered.Broadcast(ItemID);
	}
}
