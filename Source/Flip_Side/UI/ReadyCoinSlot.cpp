// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ReadyCoinSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	const FName CancelStateIconParameterName(TEXT("ReadyCoin_CancelStateIcon"));
	const FName WeaponIconParameterName(TEXT("Weapon_Icon"));
	const FName WeaponColorParameterName(TEXT("Weapon_Color"));
	const FLinearColor FrontWeaponColor(0.862745f, 0.913725f, 0.313725f, 1.0f);
	const FLinearColor BackWeaponColor(0.905882f, 0.933333f, 0.917647f, 1.0f);
}

void UReadyCoinSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ReadyCoinButton))
	{
		ReadyCoinButton->OnClicked.AddDynamic(this, &UReadyCoinSlot::HandleReadyCoinClicked);
	}
}

void UReadyCoinSlot::SetReadyCoinData(const FBattleReadyCoinViewData& InData)
{
	CoinInstanceID = InData.CoinInstanceID;
	bCanCancel = InData.bCanCancel;

	if (IsValid(ReadySlotNumberText))
	{
		ReadySlotNumberText->SetText(FText::AsNumber(InData.ReadySlotNumber));
	}

	if (IsValid(HPText))
	{
		HPText->SetText(FText::AsNumber(InData.CurrentHP));
	}

	if (IsValid(FrontWeaponIcon))
	{
		UpdateWeaponIconMaterial(
			FrontWeaponIcon,
			InData.FrontIcon,
			FrontWeaponIconMaterialInstance,
			FrontWeaponColor
		);
	}

	if (IsValid(BackWeaponIcon))
	{
		UpdateWeaponIconMaterial(
			BackWeaponIcon,
			InData.BackIcon,
			BackWeaponIconMaterialInstance,
			BackWeaponColor
		);
	}

	UpdateCancelStateVisual();

	// TODO: 코인 배경 Dynamic Material과 타입 색상 처리는 머티리얼 준비 후 추가합니다.
	// TODO: ReadyCoinSlot 호버 정보 팝업은 위젯과 표시 데이터가 준비된 뒤 추가합니다.
	SetVisibility(ESlateVisibility::Visible);
}

void UReadyCoinSlot::ClearReadyCoinData()
{
	CoinInstanceID = INDEX_NONE;
	bCanCancel = false;
	UpdateCancelStateVisual();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UReadyCoinSlot::HandleReadyCoinClicked()
{
	if (CoinInstanceID != INDEX_NONE && bCanCancel)
	{
		OnReadyCoinSlotClicked.Broadcast(CoinInstanceID);
	}
}

void UReadyCoinSlot::UpdateCancelStateVisual()
{
	if (IsValid(CanCancleText))
	{
		CanCancleText->SetText(bCanCancel ? FText::FromString(TEXT("취소 가능")) : FText::FromString(TEXT("취소 불가")));
	}

	if (!IsValid(CanCancleIcon))
	{
		return;
	}

	UTexture2D* CancelStateIconTexture = bCanCancel ? CanCancelIconTexture : CannotCancelIconTexture;
	if (!IsValid(CancelStateIconTexture))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[ReadyCoinSlot] %s icon texture is not assigned."),
			bCanCancel ? TEXT("CanCancel") : TEXT("CannotCancel")
		);
		return;
	}

	// CanCancleIcon의 BP Brush에 지정한 UI 머티리얼을 유지하고, 상태 아이콘만 교체합니다.
	CancelStateIconMaterialInstance = CanCancleIcon->GetDynamicMaterial();
	if (!IsValid(CancelStateIconMaterialInstance))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[ReadyCoinSlot] GetDynamicMaterial failed. CanCancleIcon Brush에 UI 머티리얼을 지정하세요.")
		);
		return;
	}

	CancelStateIconMaterialInstance->SetTextureParameterValue(CancelStateIconParameterName, CancelStateIconTexture);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ReadyCoinSlot] Cancel state material applied. MID=%s, Texture=%s"),
		*GetNameSafe(CancelStateIconMaterialInstance),
		*GetNameSafe(CancelStateIconTexture)
	);
}

void UReadyCoinSlot::UpdateWeaponIconMaterial(
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
			TEXT("[ReadyCoinSlot] Weapon material update skipped. Image=%s, Texture=%s"),
			*GetNameSafe(WeaponIconImage),
			*GetNameSafe(WeaponIconTexture)
		);
		return;
	}

	// WeaponIcon Image의 BP Brush에 지정한 UI 머티리얼을 유지하고 파라미터만 갱신합니다.
	DynamicMaterial = WeaponIconImage->GetDynamicMaterial();
	if (!IsValid(DynamicMaterial))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[ReadyCoinSlot] GetDynamicMaterial failed. %s Brush에 UI 머티리얼을 지정하세요."),
			*GetNameSafe(WeaponIconImage)
		);
		return;
	}

	DynamicMaterial->SetTextureParameterValue(WeaponIconParameterName, WeaponIconTexture);
	DynamicMaterial->SetVectorParameterValue(WeaponColorParameterName, WeaponColor);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ReadyCoinSlot] Weapon material applied. Image=%s, MID=%s, Texture=%s, Color=%s"),
		*GetNameSafe(WeaponIconImage),
		*GetNameSafe(DynamicMaterial),
		*GetNameSafe(WeaponIconTexture),
		*WeaponColor.ToString()
	);
}

