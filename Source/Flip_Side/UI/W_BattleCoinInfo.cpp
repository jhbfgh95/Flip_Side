#include "UI/W_BattleCoinInfo.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Internationalization/TextFormatter.h"

void UW_BattleCoinInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UW_BattleCoinInfo::UpdateBattleCoinInfo(
	UTexture2D* Icon, const FText& WeaponName, const FText& RawDescription, 
	int32 DefaultBP, int32 ModifiedBP, 
	int32 DefaultAP, int32 ModifiedAP, FLinearColor WeaponColor
    )
{
	if (HoveredWeaponIcon && Icon)
	{
		HoveredWeaponIcon->SetBrushFromTexture(Icon);
		HoveredWeaponIcon->SetColorAndOpacity(WeaponColor);
	}
	if (HoveredWeaponName)
	{
		HoveredWeaponName->SetText(WeaponName);
	}

	auto FormatStatWithDiff = [](int32 DefaultVal, int32 ModifiedVal) -> FText
	{
		FString StatStr = FString::FromInt(DefaultVal);
		
		if (ModifiedVal > 0)
		{
			StatStr += FString::Printf(TEXT(" <Green>(+%d)</>"), ModifiedVal);
		}
		else if (ModifiedVal < 0)
		{
            //Diff가 이미 음수라서 %d하면 알아서 붙음
			StatStr += FString::Printf(TEXT(" <Red>(%d)</>"), ModifiedVal);
		}
		
		return FText::FromString(StatStr);
	};

    //동적으로 포맷 정해줌
	if (HoveredWeaponDes)
	{
        //언리얼 기본 포맷 
        //사용 : {BP} 만큼 데미지를 줍니다
		FFormatNamedArguments Args;
        Args.Add(TEXT("BP"), FormatStatWithDiff(DefaultBP, ModifiedBP));
		Args.Add(TEXT("AP"), FormatStatWithDiff(DefaultAP, ModifiedAP));

		HoveredWeaponDes->SetText(FText::Format(RawDescription, Args));
	}
}