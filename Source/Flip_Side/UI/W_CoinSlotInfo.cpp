// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CoinSlotInfo.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	FText FormatReadyCoinStat(int32 Value, const TCHAR* ColorTag, const TCHAR* StatLabel)
	{
		return FText::FromString(FString::Printf(TEXT("<%s>[%s] %d</>"), ColorTag, StatLabel, Value));
	}
}

void UW_CoinSlotInfo::NativeConstruct()
{
	Super::NativeConstruct();

    if (HoveredFrontWeaponIcon && HoveredBackWeaponIcon)
    {
        FrontDynamicMaterial = HoveredFrontWeaponIcon->GetDynamicMaterial();
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

    SetWeaponInfo(
        true,
        InData.FrontIcon,
        InData.FrontWeaponName,
        InData.FrontWeaponDescription,
        InData.FrontBehaviorPoint,
        InData.FrontAttackPoint,
        InData.FrontWeaponColor
    );
    SetWeaponInfo(
        false,
        InData.BackIcon,
        InData.BackWeaponName,
        InData.BackWeaponDescription,
        InData.BackBehaviorPoint,
        InData.BackAttackPoint,
        InData.BackWeaponColor
    );
}

void UW_CoinSlotInfo::SetWeaponInfo(bool bFrontFace, UTexture2D* Icon, const FText& WeaponName, const FText& RawDescription, int32 DefaultBP, int32 DefaultAP, const FLinearColor& WeaponColor)
{
    if (bFrontFace)
    {
        if (IsValid(HoveredFrontWeaponIcon) && IsValid(Icon) && IsValid(FrontDynamicMaterial))
        {
            FrontDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), Icon);
            FrontDynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), WeaponColor);
        }
        if (IsValid(HoveredFrontWeaponName))
        {
            HoveredFrontWeaponName->SetText(WeaponName);
        }
        if (IsValid(HoveredFrontWeaponDes))
        {
            FFormatNamedArguments Args;

            //언리얼 기본 포맷 
            //사용 : {BP} 만큼 데미지를 줍니다
            Args.Add(TEXT("BP"), FormatReadyCoinStat(DefaultBP, TEXT("BPColor"), TEXT("BP")));
            Args.Add(TEXT("AP"), FormatReadyCoinStat(DefaultAP, TEXT("APColor"), TEXT("AP")));

            HoveredFrontWeaponDes->SetText(FText::Format(RawDescription, Args));
        }
    }
    else
    {
        if (IsValid(HoveredBackWeaponIcon) && IsValid(Icon) && IsValid(BackDynamicMaterial))
        {
            BackDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), Icon);
            BackDynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), WeaponColor);
        }
        if (IsValid(HoveredBackWeaponName))
        {
            HoveredBackWeaponName->SetText(WeaponName);
        }
        if (IsValid(HoveredBackWeaponDes))
        {
            //언리얼 기본 포맷 
            //사용 : {BP} 만큼 데미지를 줍니다
            FFormatNamedArguments Args;

            Args.Add(TEXT("BP"), FormatReadyCoinStat(DefaultBP, TEXT("BPColor"), TEXT("BP")));
            Args.Add(TEXT("AP"), FormatReadyCoinStat(DefaultAP, TEXT("APColor"), TEXT("AP")));
            //Args.Add(TEXT("Range"), FormatStatWithDiff(DefaultRange, ModifiedRange));

            HoveredBackWeaponDes->SetText(FText::Format(RawDescription, Args));
        }
    }
}
