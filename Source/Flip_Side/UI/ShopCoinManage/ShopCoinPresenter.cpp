// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ShopCoinManage/ShopCoinPresenter.h"

#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "UI/ShopCoinManage/W_ShopCoinSlot.h"
#include "UI/ShopCoinManage/W_ShopCoinSlotContainer.h"
#include "UI/ShopCoinManage/W_ShopWeaponSlotContainer.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "UI/ShopCoinManage/W_ShopCoinWidget.h"
#include "UI/ShopCoinManage/W_ShopCoinWeaponSlot.h"
#include "UI/ShopCoinManage/W_ShopCoinSlotBuyButton.h"
#include "UI/ShopCoinManage/W_ShopSelectCoin.h"
#include "UI/W_WeaponDescription.h"

void UShopCoinPresenter::InitPresenter(UW_ShopCoinWidget* InShopCoinWidget, UShopCoinWSubsystem* InCoinSubsystem
	,UDataManagerSubsystem* InDataManager, UUnlockGISubsystem* InUnlockSubsystem)
{
	ShopCoinWidget = InShopCoinWidget;
	CoinSubsystem = InCoinSubsystem;
	DataManager = InDataManager;
	UnlockSubsystem = InUnlockSubsystem;
	UnlockSubsystem->OnWeaponUnlock.AddDynamic(this,&UShopCoinPresenter::AddWeaponSlot );
	InitSlotWidget();
	InitWeaponSlotWidget();
	InitSlotBuyButtonWidget();
	InitSlotSelectCoin();
	IsCurrentCoinSideFront = true;
}

void UShopCoinPresenter::InitSlotWidget()
{
	ShopCoinWidget->GetShopCoinSlotContainer()->InitWidget();
	CoinSlotViews = ShopCoinWidget->GetShopCoinSlotContainer()->GetCoinSlots();
	for (UW_ShopCoinSlot* CoinSlot : CoinSlotViews)
	{
		if (IsValid(CoinSlot))
		{
			CoinSlot->OnClickedShopCoinSlot.AddDynamic(this, &UShopCoinPresenter::SelectSlot);
			CoinSlot->OnClickedShopCoinSlotFrontCoin.AddDynamic(this, &UShopCoinPresenter::SelectSlotFrontWeapon);
			CoinSlot->OnClickedShopCoinSlotBackCoin.AddDynamic(this, &UShopCoinPresenter::SelectSlotBackWeapon);
			CoinSlot->OnBuyShopCoinSlotCoin.AddDynamic(this, &UShopCoinPresenter::BuyCoinSlotCoin);
			CoinSlot->OnSellShopCoinSlotCoin.AddDynamic(this, &UShopCoinPresenter::SellCoinSlotCoin);
			CoinSlot->OnDropShopCoinSlot.AddDynamic(this, &UShopCoinPresenter::ReorderCoinSlot);
			CoinSlot->OnCancelShopCoinSlotDrag.AddDynamic(this, &UShopCoinPresenter::CancelCoinSlotReorder);
		}
	}
}
	
void UShopCoinPresenter::InitWeaponSlotWidget()
{
	TArray<int32> UnlockWeaponID = UnlockSubsystem->GetUnlockWeaponArray();
	TArray<FFaceData> UnlockWeaponData;
	for(int32 ID : UnlockWeaponID)
	{
		UnlockWeaponData.Add(GetWeaponData(ID));
	}
	ShopCoinWidget->GetShopWeaponSlotContainer()->InitWidget(UnlockWeaponData);
	WeaponSlotViews = ShopCoinWidget->GetShopWeaponSlotContainer()->GetWeaponSlots();
	for (UW_ShopCoinWeaponSlot* WeaponSlot : WeaponSlotViews)
	{
		if (IsValid(WeaponSlot))
		{
			WeaponSlot->OnClickedShopCoinWeaponSlot.AddDynamic(this, &UShopCoinPresenter::SelectWeapon);
			WeaponSlot->OnHoveredShopCoinWeaponSlot.AddDynamic(this, &UShopCoinPresenter::HoverWeapon);
			WeaponSlot->OnUnhoveredShopCoinWeaponSlot.AddDynamic(this, &UShopCoinPresenter::UnhoverWeapon);
		}
	}
}

void UShopCoinPresenter::InitSlotBuyButtonWidget()
{
	FCoinTypeStructure ShopCoinSlotData;
	int32 InPrice;
	int32 InHp;
	for(int i =1; i< 4; i++)
	{
		ShopCoinSlotData.Level = i;
		DataManager->GetCoinSlotLevelStats(ShopCoinSlotData, InPrice, InHp);
		ShopCoinWidget->GetShopCoinSlotBuyButton(i)->InitWidget(i , InPrice, InHp);
		ShopCoinWidget->GetShopCoinSlotBuyButton(i)->OnClickedShopCoinSlotBuyButton.AddDynamic(this, &
		UShopCoinPresenter::BuySlot);
	}
}
	
void UShopCoinPresenter::InitSlotSelectCoin()
{
	ShopCoinWidget->GetShopSelectCoin()->OnChangeShopSelectedCoinSide.AddDynamic(this, &UShopCoinPresenter::ChangeCoinSide);
}

void UShopCoinPresenter::SelectSlot(int32 SlotIndex)
{
	if(CurrentSelectedSlotIndex == SlotIndex)
		return;
	if(!CoinSubsystem->GetIsCoinSlotUnlockByIndex(SlotIndex))
		return;

	CurrentSelectedSlotIndex = SlotIndex;
	SetCoinSideFront(true);

	SetSlectCoinSideData(SlotIndex);

}
	
void UShopCoinPresenter::SelectSlotFrontWeapon(int32 SlotIndex)
{
	if(!CoinSubsystem->GetIsCoinSlotUnlockByIndex(SlotIndex))
		return;

	CurrentSelectedSlotIndex = SlotIndex;
	SetCoinSideFront(true);

	SetSlectCoinSideData(SlotIndex);
}

void UShopCoinPresenter::SelectSlotBackWeapon(int32 SlotIndex)
{
	if(!CoinSubsystem->GetIsCoinSlotUnlockByIndex(SlotIndex))
		return;

	CurrentSelectedSlotIndex = SlotIndex;
	SetCoinSideFront(false);
	
	SetSlectCoinSideData(SlotIndex);
}

void UShopCoinPresenter::HoverSlot(int32 SlotIndex)
{
	SetSlectCoinSideData(SlotIndex);
}

void UShopCoinPresenter::UnhoverSlot()
{
	if(CurrentSelectedSlotIndex == -1)
		return;
	SetSlectCoinSideData(CurrentSelectedSlotIndex);
}


void UShopCoinPresenter::SelectWeapon(int32 WeaponID)
{
	if(CurrentSelectedSlotIndex == -1)
		return;

	const FFaceData WeaponData = GetWeaponData(WeaponID);
	UpdateWeaponDescription(WeaponData);

	if(IsCurrentCoinSideFront)
	{
		CoinSubsystem->SetCoinSlotFrontWeapon(CurrentSelectedSlotIndex, WeaponID);
		CoinSlotViews[CurrentSelectedSlotIndex]->SetFrontWeaponImage(WeaponData);
	}
	else
	{
		CoinSubsystem->SetCoinSlotBackWeapon(CurrentSelectedSlotIndex, WeaponID);
		CoinSlotViews[CurrentSelectedSlotIndex]->SetBackWeaponImage(WeaponData);
	}
	
	SetSlectCoinSideData(CurrentSelectedSlotIndex);
		
}
	
void UShopCoinPresenter::HoverWeapon(int32 WeaponID)
{
	UE_LOG(LogTemp,Warning, TEXT("호버링 중인 무기 %d"), WeaponID);
	const FFaceData WeaponData = GetWeaponData(WeaponID);
	UpdateWeaponDescription(WeaponData);

	if(IsCurrentCoinSideFront)
		ShopCoinWidget->GetShopSelectCoin()->SetFrontWeapon(WeaponData);
	else
		ShopCoinWidget->GetShopSelectCoin()->SetBackWeapon(WeaponData);

}

void UShopCoinPresenter::UnhoverWeapon()
{
	HideWeaponDescription();
	if(CurrentSelectedSlotIndex == -1)
		return;

	FCoinTypeStructure IndexCoin = CoinSubsystem->GetCoinSlotCoinType(CurrentSelectedSlotIndex);
	
	if(IsCurrentCoinSideFront)
		ShopCoinWidget->GetShopSelectCoin()->SetFrontWeapon(GetWeaponData(IndexCoin.FrontWeaponID));
	else
		ShopCoinWidget->GetShopSelectCoin()->SetBackWeapon(GetWeaponData(IndexCoin.BackWeaponID));
}

void UShopCoinPresenter::BuySlot(int32 Level)
{
	if(CoinSubsystem->BuyCoinSlot(Level))
	{
		FCoinTypeStructure InCoinData;
		InCoinData.Level = Level;
		int32 Cost;
		int32 Hp;
		DataManager->GetCoinSlotLevelStats(InCoinData, Cost, Hp);
		ShopCoinWidget->GetShopCoinSlotContainer()->AddCoinSlot(CoinSubsystem->GetUnlockCoinSlotCount()-1, Hp);
		SelectSlot(CoinSubsystem->GetUnlockCoinSlotCount()-1);

	}
}
	
void UShopCoinPresenter::BuyCoinSlotCoin(int32 SlotIndex, int32 Count)
{
	if(!CoinSubsystem->GetIsCoinSlotUnlockByIndex(SlotIndex) || !CoinSlotViews.IsValidIndex(SlotIndex))
		return;

	CoinSubsystem->IncreaseCoinSlotCoin(SlotIndex, Count);
	CoinSlotViews[SlotIndex]->SetCountText(
		CoinSubsystem->GetCoinSlotCoinType(SlotIndex).SameTypeCoinNum);
}

void UShopCoinPresenter::SellCoinSlotCoin(int32 SlotIndex, int32 Count)
{
	if(!CoinSubsystem->GetIsCoinSlotUnlockByIndex(SlotIndex) || !CoinSlotViews.IsValidIndex(SlotIndex))
		return;

	CoinSubsystem->DecreaseCoinSlotCoin(SlotIndex, Count);
	CoinSlotViews[SlotIndex]->SetCountText(
		CoinSubsystem->GetCoinSlotCoinType(SlotIndex).SameTypeCoinNum);
}
	
void UShopCoinPresenter::SetSlectCoinSideData(int32 SlotIndex)
{
	FCoinTypeStructure IndexCoin = CoinSubsystem->GetCoinSlotCoinType(SlotIndex);
	ShopCoinWidget->GetShopSelectCoin()->SetFrontWeapon(GetWeaponData(IndexCoin.FrontWeaponID));
	ShopCoinWidget->GetShopSelectCoin()->SetBackWeapon(GetWeaponData(IndexCoin.BackWeaponID));
}

void UShopCoinPresenter::AddWeaponSlot(int32 WeaponID)
{
	if(WeaponID == -1)
		return;

	const FFaceData WeaponData = GetWeaponData(WeaponID);
	if(WeaponData.WeaponID == -1)
		return;

	UW_ShopCoinWeaponSlot* NewWeaponSlot =
		ShopCoinWidget->GetShopWeaponSlotContainer()->AddWeaponSlot(WeaponData);
	if(!IsValid(NewWeaponSlot))
		return;

	WeaponSlotViews.Add(NewWeaponSlot);
	NewWeaponSlot->OnClickedShopCoinWeaponSlot.AddDynamic(
		this, &UShopCoinPresenter::SelectWeapon);
	NewWeaponSlot->OnHoveredShopCoinWeaponSlot.AddDynamic(
		this, &UShopCoinPresenter::HoverWeapon);
	NewWeaponSlot->OnUnhoveredShopCoinWeaponSlot.AddDynamic(
		this, &UShopCoinPresenter::UnhoverWeapon);
}

void UShopCoinPresenter::ReorderCoinSlot(int32 SourceIndex, int32 TargetIndex)
{
	ShopCoinWidget->GetShopCoinSlotContainer()->ResetSlotPreview();

	if(!CoinSubsystem->MoveCoinSlot(SourceIndex, TargetIndex))
		return;

	UE_LOG(LogTemp, Warning, TEXT("변경전 인덱스 %d, 변경 후 인덱스 %d"), SourceIndex, TargetIndex);
	if(CurrentSelectedSlotIndex == SourceIndex)
	{
		CurrentSelectedSlotIndex = TargetIndex;
	}
	else if(SourceIndex < TargetIndex &&
		CurrentSelectedSlotIndex > SourceIndex &&
		CurrentSelectedSlotIndex <= TargetIndex)
	{
		--CurrentSelectedSlotIndex;
	}
	else if(TargetIndex < SourceIndex &&
		CurrentSelectedSlotIndex >= TargetIndex &&
		CurrentSelectedSlotIndex < SourceIndex)
	{
		++CurrentSelectedSlotIndex;
	}

	RefreshCoinSlots();
}

void UShopCoinPresenter::CancelCoinSlotReorder()
{
	if(IsValid(ShopCoinWidget))
	{
		ShopCoinWidget->GetShopCoinSlotContainer()->ResetSlotPreview();
	}
}

void UShopCoinPresenter::RefreshCoinSlots()
{
	for(int32 Index = 0; Index < CoinSlotViews.Num(); ++Index)
	{
		UW_ShopCoinSlot* SlotView = CoinSlotViews[Index];
		if(!IsValid(SlotView) || !CoinSubsystem->GetIsCoinSlotUnlockByIndex(Index))
			continue;

		const FCoinTypeStructure CoinData = CoinSubsystem->GetCoinSlotCoinType(Index);
		SlotView->InitSlotWidget(Index);
		SlotView->SetCountText(CoinData.SameTypeCoinNum);
		SlotView->SetFrontWeaponImage(GetWeaponData(CoinData.FrontWeaponID));
		SlotView->SetBackWeaponImage(GetWeaponData(CoinData.BackWeaponID));

		int32 Price = 0;
		int32 Hp = 0;
		if(DataManager->GetCoinSlotLevelStats(CoinData, Price, Hp))
		{
			SlotView->SetCoinSlot(Hp);
		}
	}

	if(CurrentSelectedSlotIndex != INDEX_NONE)
	{
		SetSlectCoinSideData(CurrentSelectedSlotIndex);
	}
}

FFaceData UShopCoinPresenter::GetWeaponData(int32 WeaponID)
{
	FFaceData ReturnData;
	ReturnData.WeaponID = -1;
	DataManager->TryGetWeapon(WeaponID, ReturnData);
	return ReturnData;
}

void UShopCoinPresenter::UpdateWeaponDescription(const FFaceData& WeaponData)
{
	if(!IsValid(ShopCoinWidget) || WeaponData.WeaponID == -1)
	{
		HideWeaponDescription();
		return;
	}

	if(UW_WeaponDescription* DescriptionWidget = ShopCoinWidget->GetWeaponDescription())
	{
		DescriptionWidget->SetExplainText(
			WeaponData.WeaponName,
			WeaponData.KOR_DES,
			WeaponData.BehaviorPoint,
			WeaponData.AttackPoint);
	}
}

void UShopCoinPresenter::HideWeaponDescription()
{
	if(IsValid(ShopCoinWidget))
	{
		if(UW_WeaponDescription* DescriptionWidget = ShopCoinWidget->GetWeaponDescription())
		{
			DescriptionWidget->SetExplainTextEmpty();
		}
	}
}
	
void UShopCoinPresenter::SetCoinSideFront(bool SetFront)
{
	if(IsCurrentCoinSideFront == SetFront)
		return;

	IsCurrentCoinSideFront = SetFront;
	ShopCoinWidget->GetShopSelectCoin()->ChangeCoinSide(IsCurrentCoinSideFront);
}
	
void UShopCoinPresenter::ChangeCoinSide()
{
	SetCoinSideFront(!IsCurrentCoinSideFront);
}
