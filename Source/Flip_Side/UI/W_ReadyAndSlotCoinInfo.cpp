// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ReadyAndSlotCoinInfo.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

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
            FString BPString = FString::Printf(TEXT("<White>%d</>"), DefaultBP);
            FString APString = FString::Printf(TEXT("<White>%d</>"), DefaultAP);

            Args.Add(TEXT("BP"), FText::FromString(BPString));
            Args.Add(TEXT("AP"), FText::FromString(APString));

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

            FString BPString = FString::Printf(TEXT("<White>%d</>"), DefaultBP);
            FString APString = FString::Printf(TEXT("<White>%d</>"), DefaultAP);

            Args.Add(TEXT("BP"), FText::FromString(BPString));
            Args.Add(TEXT("AP"), FText::FromString(APString));
            //Args.Add(TEXT("Range"), FormatStatWithDiff(DefaultRange, ModifiedRange));

            HoveredBackWeaponDes->SetText(FText::Format(RawDescription, Args));
        }
    }
}