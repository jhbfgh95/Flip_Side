#include "UI/BattleCardSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	const FName CardIconParameterName(TEXT("Weapon_Icon"));
	const FName CardColorParameterName(TEXT("Weapon_Color"));
	const FLinearColor CardIconColor(0.670588f, 0.541176f, 1.0f, 1.0f); // AB8AFFFF
}

void UBattleCardSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(CardSlotButton))
	{
		CardSlotButton->OnHovered.AddDynamic(this, &UBattleCardSlotWidget::HandleCardSlotHovered);
		CardSlotButton->OnUnhovered.AddDynamic(this, &UBattleCardSlotWidget::HandleCardSlotUnhovered);
	}
}

void UBattleCardSlotWidget::SetCardData(const FBattleCardSlotViewData& InData)
{
	SlotNumber = InData.SlotNumber;
	bOccupied = InData.bOccupied;

	if (IsValid(CardIcon) && IsValid(InData.CardData.Icon))
	{
		// CardIcon의 BP Brush에 지정한 아이템/무기 공용 UI 머티리얼을 유지하고 파라미터만 갱신합니다.
		CardIconMaterialInstance = CardIcon->GetDynamicMaterial();
		if (IsValid(CardIconMaterialInstance))
		{
			CardIconMaterialInstance->SetTextureParameterValue(CardIconParameterName, InData.CardData.Icon);
			CardIconMaterialInstance->SetVectorParameterValue(CardColorParameterName, CardIconColor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[BattleCardSlot] GetDynamicMaterial failed. CardIcon Brush에 공용 UI 머티리얼을 지정하세요."));
		}
	}

	if (IsValid(CardNameText))
	{
		CardNameText->SetText(FText::FromString(InData.CardData.CardName));
	}

	// TODO: Tick 기반 조건 검사 이관 후 InData.bIsActive로 카드 활성 연출을 갱신합니다.
	SetVisibility(bOccupied ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UBattleCardSlotWidget::ClearCardData()
{
	SlotNumber = INDEX_NONE;
	bOccupied = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UBattleCardSlotWidget::HandleCardSlotHovered()
{
	if (bOccupied && SlotNumber != INDEX_NONE)
	{
		OnBattleCardSlotHovered.Broadcast(SlotNumber);
	}
}

void UBattleCardSlotWidget::HandleCardSlotUnhovered()
{
	if (SlotNumber != INDEX_NONE)
	{
		OnBattleCardSlotUnhovered.Broadcast(SlotNumber);
	}
}
