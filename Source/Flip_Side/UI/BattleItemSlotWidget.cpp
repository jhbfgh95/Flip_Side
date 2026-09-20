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
		ItemButton->OnClicked.AddUniqueDynamic(this, &UBattleItemSlotWidget::HandleItemButtonClicked);
		ItemButton->OnHovered.AddUniqueDynamic(this, &UBattleItemSlotWidget::HandleItemButtonHovered);
		ItemButton->OnUnhovered.AddUniqueDynamic(this, &UBattleItemSlotWidget::HandleItemButtonUnhovered);
	}
	RefreshInfoSelectionStyle();
	OnInfoSelectionChanged(bIsInfoSelected);
}

void UBattleItemSlotWidget::NativeDestruct()
{
	if (IsValid(ItemButton))
	{
		ItemButton->OnClicked.RemoveDynamic(this, &UBattleItemSlotWidget::HandleItemButtonClicked);
		ItemButton->OnHovered.RemoveDynamic(this, &UBattleItemSlotWidget::HandleItemButtonHovered);
		ItemButton->OnUnhovered.RemoveDynamic(this, &UBattleItemSlotWidget::HandleItemButtonUnhovered);
		if (bHasUnselectedButtonStyle) ItemButton->SetStyle(UnselectedButtonStyle);
	}
	bHasUnselectedButtonStyle = false;
	Super::NativeDestruct();
}

void UBattleItemSlotWidget::SetInfoSelected(bool bSelected)
{
	bSelected = bSelected && ItemID != INDEX_NONE && AvailableCount > 0;
	if (bIsInfoSelected == bSelected) return;
	bIsInfoSelected = bSelected;
	RefreshInfoSelectionStyle();
	OnInfoSelectionChanged(bIsInfoSelected);
}

void UBattleItemSlotWidget::RefreshInfoSelectionStyle()
{
	if (!IsValid(ItemButton) || !bUseHoveredStyleForInfoSelection) return;
	if (!bHasUnselectedButtonStyle)
	{
		UnselectedButtonStyle = ItemButton->GetStyle();
		bHasUnselectedButtonStyle = true;
	}
	FButtonStyle DisplayStyle = UnselectedButtonStyle;
	if (bIsInfoSelected)
	{
		DisplayStyle.Normal = UnselectedButtonStyle.Hovered;
		DisplayStyle.NormalForeground = UnselectedButtonStyle.HoveredForeground;
	}
	ItemButton->SetStyle(DisplayStyle);
}

void UBattleItemSlotWidget::SetItemData(const FBattleItemSlotViewData& InData)
{
	if (ItemID != InData.ItemData.ItemID || InData.AvailableCount <= 0) SetInfoSelected(false);
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
	SetInfoSelected(false);
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
