// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ReadyAndSlotCoinInfo.h"
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

void UW_ReadyAndSlotCoinInfo::NativeConstruct()
{
	Super::NativeConstruct();

    if (HoveredFrontWeaponIcon && HoveredBackWeaponIcon)
    {
        FrontDynamicMaterial = HoveredFrontWeaponIcon->GetDynamicMaterial();
        BackDynamicMaterial = HoveredBackWeaponIcon->GetDynamicMaterial();
    }
}

void UW_ReadyAndSlotCoinInfo::SetReadyCoinInfo(bool FaceFlag, UTexture2D * Icon, const FText & WeaponName, const FText & RawDescription, int32 DefaultBP, int32 DefaultAP, FLinearColor typeColor)
{
    if(FaceFlag)
    {
        if (HoveredFrontWeaponIcon && Icon && FrontDynamicMaterial)
        {
            FrontDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), Icon);
            FrontDynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), typeColor);
        }
        if (HoveredFrontWeaponName)
        {
            HoveredFrontWeaponName->SetText(WeaponName);
        }
        if (HoveredFrontWeaponDes)
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
        if (HoveredBackWeaponIcon && Icon && BackDynamicMaterial)
        {
            BackDynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), Icon);
            BackDynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), typeColor);
        }
        if (HoveredBackWeaponName)
        {
            HoveredBackWeaponName->SetText(WeaponName);
        }
        if (HoveredBackWeaponDes)
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
