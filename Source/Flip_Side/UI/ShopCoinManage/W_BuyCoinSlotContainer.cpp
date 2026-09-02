// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_BuyCoinSlotContainer.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/ShopCoinManage/W_ShopCoinSlotBuyButton.h"

void UW_BuyCoinSlotContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(OpenSlotBuyPopupButton))
	{
		OpenSlotBuyPopupButton->OnClicked.AddDynamic(this, &ThisClass::ClickOpenSlotBuyPopup);
	}

	CloseSlotBuyPopupBorder();
}

void UW_BuyCoinSlotContainer::InitWidget(const TArray<FCoinTypeStructure>& InShopCoinSlotData,
	UDataManagerSubsystem* InDataManager)
{
	if (!IsValid(SlotBuyButtonBox) || !IsValid(ShopCoinSlotBuyButtonClass) || !IsValid(InDataManager))
	{
		return;
	}

	SlotBuyButtonBox->ClearChildren();
	SlotBuyButtons.Reset();

	for (const FCoinTypeStructure& ShopCoinSlotData : InShopCoinSlotData)
	{
		int32 SlotPrice = 0;
		int32 SlotHp = 0;
		if (!InDataManager->GetCoinSlotLevelStats(ShopCoinSlotData, SlotPrice, SlotHp))
		{
			continue;
		}

		UW_ShopCoinSlotBuyButton* SlotBuyButton = CreateWidget<UW_ShopCoinSlotBuyButton>(
			this, ShopCoinSlotBuyButtonClass);
		if (!IsValid(SlotBuyButton))
		{
			continue;
		}

		SlotBuyButton->InitWidget(ShopCoinSlotData.Level, SlotPrice, SlotHp);
		SlotBuyButton->OnClickedShopCoinSlotBuyButton.AddDynamic(
			this, &ThisClass::ClickSlotBuyButton);
		SlotBuyButtons.Add(SlotBuyButton);

		if (UVerticalBoxSlot* VerticalSlot = SlotBuyButtonBox->AddChildToVerticalBox(SlotBuyButton))
		{
			VerticalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			VerticalSlot->SetHorizontalAlignment(HAlign_Center);
		}
	}
}

void UW_BuyCoinSlotContainer::OpenSlotBuyPopupBorder()
{
	if (IsValid(SlotBuyBorder))
	{
		IsPopupOpen = true;
		SlotBuyBorder->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_BuyCoinSlotContainer::CloseSlotBuyPopupBorder()
{
	if (IsValid(SlotBuyBorder))
	{
		IsPopupOpen = false;
		SlotBuyBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UW_BuyCoinSlotContainer::ClickOpenSlotBuyPopup()
{
	if(!IsPopupOpen)
	{
		SlotBuyButtonText->SetText(FText::FromString(TEXT("취소")));
		OpenSlotBuyPopupBorder();
	}
	else
	{
		SlotBuyButtonText->SetText(FText::FromString(TEXT("슬롯 구매")));
		CloseSlotBuyPopupBorder();
	}
		
}

void UW_BuyCoinSlotContainer::ClickCancelButton()
{
	CloseSlotBuyPopupBorder();
	OnBuyCoinSlotPopupCancelled.Broadcast();
}

void UW_BuyCoinSlotContainer::ClickSlotBuyButton(int32 SlotLevel)
{
	OnBuyCoinSlotRequested.Broadcast(SlotLevel);
}

