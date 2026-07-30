#include "UI/BattleCardSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

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
		CardIcon->SetBrushFromTexture(InData.CardData.Icon, false);
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
