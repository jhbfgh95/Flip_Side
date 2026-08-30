// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ReadyCoinSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	const FName CancelStateIconParameterName(TEXT("ReadyCoin_CancelStateIcon"));
	const FName ReadyCoinWeaponIconParameterName(TEXT("Weapon_Icon"));
	const FName ReadyCoinWeaponColorParameterName(TEXT("Weapon_Color"));
	const FLinearColor ReadyCoinFrontWeaponColor(0.862745f, 0.913725f, 0.313725f, 1.0f);
	const FLinearColor ReadyCoinBackWeaponColor(0.905882f, 0.933333f, 0.917647f, 1.0f);
}

void UReadyCoinSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ReadyCoinButton))
	{
		ReadyCoinButton->OnClicked.RemoveDynamic(this, &UReadyCoinSlot::HandleReadyCoinClicked);
		ReadyCoinButton->OnHovered.RemoveDynamic(this, &UReadyCoinSlot::HandleReadyCoinHovered);
		ReadyCoinButton->OnUnhovered.RemoveDynamic(this, &UReadyCoinSlot::HandleReadyCoinUnhovered);
		ReadyCoinButton->OnClicked.AddDynamic(this, &UReadyCoinSlot::HandleReadyCoinClicked);
		ReadyCoinButton->OnHovered.AddDynamic(this, &UReadyCoinSlot::HandleReadyCoinHovered);
		ReadyCoinButton->OnUnhovered.AddDynamic(this, &UReadyCoinSlot::HandleReadyCoinUnhovered);
	}
}

void UReadyCoinSlot::SetReadyCoinData(const FBattleReadyCoinViewData& InData)
{
	CoinInstanceID = InData.CoinInstanceID;
	bCanCancel = InData.bCanCancel;

	if (IsValid(FrontWeaponIcon))
	{
		FrontWeaponIcon->SetVisibility(ESlateVisibility::Visible);
	}

	if (IsValid(BackWeaponIcon))
	{
		BackWeaponIcon->SetVisibility(ESlateVisibility::Visible);
	}

	if (IsValid(HPText))
	{
		HPText->SetVisibility(ESlateVisibility::Visible);
	}

	if (IsValid(CanCancleText))
	{
		CanCancleText->SetVisibility(ESlateVisibility::Visible);
	}

	if (IsValid(CanCancleIcon))
	{
		CanCancleIcon->SetVisibility(ESlateVisibility::Visible);
	}

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
			ReadyCoinFrontWeaponColor
		);
	}

	if (IsValid(BackWeaponIcon))
	{
		UpdateWeaponIconMaterial(
			BackWeaponIcon,
			InData.BackIcon,
			BackWeaponIconMaterialInstance,
			ReadyCoinBackWeaponColor
		);
	}

	UpdateCancelStateVisual();

	// TODO: 코인 배경 Dynamic Material과 타입 색상 처리는 머티리얼 준비 후 추가합니다.
	// TODO: ReadyCoinSlot 호버 정보 팝업은 위젯과 표시 데이터가 준비된 뒤 추가합니다.
	SetVisibility(ESlateVisibility::Visible);
}

void UReadyCoinSlot::ClearReadyCoinData()
{
	if (CoinInstanceID != INDEX_NONE)
	{
		OnReadyCoinSlotUnhovered.Broadcast(CoinInstanceID);
	}
	CoinInstanceID = INDEX_NONE;
	bCanCancel = false;

	// 빈 슬롯도 Horizontal Box의 고정 폭을 유지해야 하므로 슬롯 위젯 자체는 숨기지 않습니다.
	if (IsValid(FrontWeaponIcon))
	{
		FrontWeaponIcon->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(BackWeaponIcon))
	{
		BackWeaponIcon->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(HPText))
	{
		HPText->SetText(FText::GetEmpty());
		HPText->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(CanCancleText))
	{
		CanCancleText->SetText(FText::GetEmpty());
		CanCancleText->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(CanCancleIcon))
	{
		CanCancleIcon->SetVisibility(ESlateVisibility::Hidden);
	}

	SetVisibility(ESlateVisibility::Visible);
}

void UReadyCoinSlot::HandleReadyCoinClicked()
{
	if (CoinInstanceID != INDEX_NONE && bCanCancel)
	{
		OnReadyCoinSlotClicked.Broadcast(CoinInstanceID);
	}
}

void UReadyCoinSlot::HandleReadyCoinHovered()
{
	if (CoinInstanceID != INDEX_NONE)
	{
		OnReadyCoinSlotHovered.Broadcast(CoinInstanceID);
	}
}

void UReadyCoinSlot::HandleReadyCoinUnhovered()
{
	if (CoinInstanceID != INDEX_NONE)
	{
		OnReadyCoinSlotUnhovered.Broadcast(CoinInstanceID);
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

	DynamicMaterial->SetTextureParameterValue(ReadyCoinWeaponIconParameterName, WeaponIconTexture);
	DynamicMaterial->SetVectorParameterValue(ReadyCoinWeaponColorParameterName, WeaponColor);
}

