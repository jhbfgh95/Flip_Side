#include "UI/W_BattleCoinInfo.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

namespace
{
	FText FormatStatWithDiff(int32 DefaultVal, int32 ModifiedVal, const TCHAR* DefaultColorTag, const TCHAR* StatLabel)
	{
		FString StatStr = FString::Printf(TEXT("<%s>[%s] %d</>"), DefaultColorTag, StatLabel, DefaultVal);

		if (ModifiedVal > 0)
		{
			StatStr += FString::Printf(TEXT(" <Green>(+%d)</>"), ModifiedVal);
		}
		else if (ModifiedVal < 0)
		{
			StatStr += FString::Printf(TEXT(" <Red>(%d)</>"), ModifiedVal);
		}

		return FText::FromString(StatStr);
	}

	FText BuildWeaponDescription(
		const FText& DescriptionSource,
		int32 DefaultBP,
		int32 ModifiedBP,
		int32 DefaultAP,
		int32 ModifiedAP,
		int32 AbsorbedBP,
		bool bAppendAbsorbedBP
	)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("BP"), FormatStatWithDiff(DefaultBP, ModifiedBP, TEXT("BPColor"), TEXT("BP")));
		Args.Add(TEXT("AP"), FormatStatWithDiff(DefaultAP, ModifiedAP + AbsorbedBP, TEXT("APColor"), TEXT("AP")));
		Args.Add(TEXT("AbsorbedBP"), FText::AsNumber(AbsorbedBP));

		FText DescriptionText = FText::Format(DescriptionSource, Args);
		if (bAppendAbsorbedBP && AbsorbedBP != 0)
		{
			const TCHAR* AbsorbColorTag = AbsorbedBP > 0 ? TEXT("Green") : TEXT("Red");
			const FString AbsorbSign = AbsorbedBP > 0 ? TEXT("+") : TEXT("");
			DescriptionText = FText::FromString(FString::Printf(
				TEXT("%s\n<BPColor>[BP Absorb]</> <%s>%s%d</>"),
				*DescriptionText.ToString(),
				AbsorbColorTag,
				*AbsorbSign,
				AbsorbedBP
			));
		}

		return DescriptionText;
	}
}

void UW_BattleCoinInfo::NativeConstruct()
{
	Super::NativeConstruct();

	if (HoveredWeaponIcon)
	{
		DynamicMaterial = HoveredWeaponIcon->GetDynamicMaterial();
	}

	CacheBuffIconImages();
}

void UW_BattleCoinInfo::UpdateBattleCoinInfo(
	UTexture2D* Icon,
	const FText& WeaponName,
	const FText& RawDescription,
	int32 DefaultBP,
	int32 ModifiedBP,
	int32 DefaultAP,
	int32 ModifiedAP,
	int32 CurrentHP,
	int32 MaxHP,
	FLinearColor WeaponColor,
	const TArray<FBuffInfo>& ActiveBuffs,
	int32 AbsorbedBP,
	int32 SlotIndex,
	const FBattleCoinAlternateFaceInfo& AlternateFaceInfo
)
{
	(void)AlternateFaceInfo;

	if (HoveredWeaponIcon && Icon && DynamicMaterial)
	{
		DynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), Icon);
		DynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), WeaponColor);
	}

	if (HoveredWeaponName)
	{
		HoveredWeaponName->SetText(WeaponName);
	}

	if (CoinCurrentHPText)
	{
		CoinCurrentHPText->SetText(FText::AsNumber(CurrentHP));
	}

	if (CoinMaxHPText)
	{
		CoinMaxHPText->SetText(FText::AsNumber(MaxHP));
	}

	if (CoinSlotIndexText)
	{
		CoinSlotIndexText->SetText(FText::FromString(FString::Printf(TEXT("#%d"), SlotIndex)));
	}

	if (HoveredWeaponDes)
	{
		HoveredWeaponDes->SetText(BuildWeaponDescription(
			RawDescription,
			DefaultBP,
			ModifiedBP,
			DefaultAP,
			ModifiedAP,
			AbsorbedBP,
			true
		));
	}

	UpdateBuffIcons(ActiveBuffs);
}

void UW_BattleCoinInfo::CacheBuffIconImages()
{
	BuffIconImages.Empty();

	auto AddBuffImage = [this](UImage* Image)
	{
		if (Image && Image != HoveredWeaponIcon)
		{
			BuffIconImages.AddUnique(Image);
			Image->SetVisibility(ESlateVisibility::Collapsed);
		}
	};

	AddBuffImage(HoveredBuffIcon1);
	AddBuffImage(HoveredBuffIcon2);
	AddBuffImage(HoveredBuffIcon3);
	AddBuffImage(HoveredBuffIcon4);
	AddBuffImage(HoveredBuffIcon5);

	if (!WidgetTree)
	{
		return;
	}

	WidgetTree->ForEachWidget([&AddBuffImage](UWidget* Widget)
	{
		UImage* Image = Cast<UImage>(Widget);
		if (!Image)
		{
			return;
		}

		const FString WidgetName = Image->GetName();
		const bool bLooksLikeBuffIcon =
			(WidgetName.Contains(TEXT("Buff")) || WidgetName.Contains(TEXT("buff"))) &&
			(WidgetName.Contains(TEXT("Icon")) || WidgetName.Contains(TEXT("icon")));

		if (bLooksLikeBuffIcon)
		{
			AddBuffImage(Image);
		}
	});
}

void UW_BattleCoinInfo::SetBuffIconImage(UImage* BuffImage, UTexture2D* Icon)
{
	if (!BuffImage)
	{
		return;
	}

	if (!Icon)
	{
		BuffImage->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	if (UMaterialInstanceDynamic* BuffMaterial = BuffImage->GetDynamicMaterial())
	{
		BuffMaterial->SetTextureParameterValue(FName("Weapon_Icon"), Icon);
	}
	else
	{
		BuffImage->SetBrushFromTexture(Icon);
	}

	BuffImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UW_BattleCoinInfo::UpdateBuffIcons(const TArray<FBuffInfo>& ActiveBuffs)
{
	if (BuffIconImages.IsEmpty())
	{
		CacheBuffIconImages();
	}

	int32 IconIndex = 0;
	for (const FBuffInfo& BuffInfo : ActiveBuffs)
	{
		if (!BuffInfo.BuffIcon)
		{
			continue;
		}

		if (!BuffIconImages.IsValidIndex(IconIndex))
		{
			break;
		}

		SetBuffIconImage(BuffIconImages[IconIndex], BuffInfo.BuffIcon);
		IconIndex++;
	}

	for (; IconIndex < BuffIconImages.Num(); ++IconIndex)
	{
		SetBuffIconImage(BuffIconImages[IconIndex], nullptr);
	}
}
