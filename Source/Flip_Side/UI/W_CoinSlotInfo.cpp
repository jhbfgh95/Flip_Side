#include "UI/W_CoinSlotInfo.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

void UW_CoinSlotInfo::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(HoveredFrontWeaponIcon))
	{
		FrontDynamicMaterial = HoveredFrontWeaponIcon->GetDynamicMaterial();
	}
	if (IsValid(HoveredBackWeaponIcon))
	{
		BackDynamicMaterial = HoveredBackWeaponIcon->GetDynamicMaterial();
	}
}

void UW_CoinSlotInfo::SetCoinSlotInfo(const FBattleCoinSlotViewData& InData)
{
	if (IsValid(CoinSlotNumberText))
	{
		CoinSlotNumberText->SetText(FText::AsNumber(InData.SlotNumber));
	}
	if (IsValid(CoinCountText))
	{
		CoinCountText->SetText(FText::AsNumber(InData.CoinCount));
	}
	if (IsValid(CoinMaxHPText))
	{
		CoinMaxHPText->SetText(FText::AsNumber(InData.HP));
	}

	SetWeaponInfo(true, InData.FrontIcon, InData.FrontWeaponName, InData.FrontWeaponStats, InData.FrontWeaponColor);
	SetWeaponInfo(false, InData.BackIcon, InData.BackWeaponName, InData.BackWeaponStats, InData.BackWeaponColor);
}

void UW_CoinSlotInfo::SetWeaponInfo(
	bool bFrontFace,
	UTexture2D* Icon,
	const FText& WeaponName,
	const FWeaponStatDisplayData& StatData,
	const FLinearColor& WeaponColor)
{
	UImage* WeaponIcon = bFrontFace ? HoveredFrontWeaponIcon : HoveredBackWeaponIcon;
	UTextBlock* WeaponNameText = bFrontFace ? HoveredFrontWeaponName : HoveredBackWeaponName;
	UMaterialInstanceDynamic* DynamicMaterial = bFrontFace ? FrontDynamicMaterial : BackDynamicMaterial;
	UTextBlock* AttackPowerText = bFrontFace ? FrontAttackPowerText : BackAttackPowerText;
	UTextBlock* WeaponPowerText = bFrontFace ? FrontWeaponPowerText : BackWeaponPowerText;
	UTextBlock* CountText = bFrontFace ? FrontCountText : BackCountText;

	if (IsValid(WeaponIcon) && IsValid(Icon) && IsValid(DynamicMaterial))
	{
		DynamicMaterial->SetTextureParameterValue(FName(TEXT("Weapon_Icon")), Icon);
		DynamicMaterial->SetVectorParameterValue(FName(TEXT("Weapon_Color")), WeaponColor);
	}
	if (IsValid(WeaponNameText))
	{
		WeaponNameText->SetText(WeaponName);
	}
	if (IsValid(AttackPowerText))
	{
		AttackPowerText->SetText(FText::AsNumber(StatData.AttackPower));
	}
	if (IsValid(WeaponPowerText))
	{
		WeaponPowerText->SetText(FText::AsNumber(StatData.WeaponPower));
	}
	if (IsValid(CountText))
	{
		CountText->SetText(FText::AsNumber(StatData.Count));
	}
}
