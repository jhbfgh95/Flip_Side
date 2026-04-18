#include "UI/W_ItemInfo.h"

#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "DataTypes/ItemDataTypes.h"
#include "Materials/MaterialInstanceDynamic.h"

void UW_ItemInfo::NativeConstruct()
{
	Super::NativeConstruct();

	if (HoveredItemIcon)
	{
		DynamicMaterial = HoveredItemIcon->GetDynamicMaterial();
	}
}

void UW_ItemInfo::UpdateItemInfo(UTexture2D* Icon, const FText& ItemName, const FText& Description, FLinearColor ItemColor)
{
	if (HoveredItemIcon && Icon)
	{
		if (DynamicMaterial)
		{
			//이거 머테리얼 무기쪽이랑 공유한다. 어짜피 기능은 똑같기 때문.
			DynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), Icon);
			DynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), ItemColor);
		}
		else
		{
			HoveredItemIcon->SetBrushFromTexture(Icon);
		}
	}

	if (HoveredItemName)
	{
		HoveredItemName->SetText(ItemName);
	}

	SetDescriptionText(Description);
}

void UW_ItemInfo::UpdateItemInfo(const FItemData& ItemData)
{
	UpdateItemInfo(
		ItemData.ItemIcon,
		FText::FromString(ItemData.ItemName),
		FText::FromString(ItemData.Item_DES),
		ItemData.TypeColor
	);
}

void UW_ItemInfo::SetDescriptionText(const FText& Description)
{
	if (HoveredItemDes)
	{
		HoveredItemDes->SetText(Description);
	}

	if (HoveredItemRichDes)
	{
		HoveredItemRichDes->SetText(Description);
	}
}
