// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinManage.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/W_WeaponDescription.h"
#include "Components/Button.h"
void UW_ShopCoinManage::NativeConstruct()
{
    Super::NativeConstruct();

    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

    
	CoinSubsystem->OnChangeSlotCoinSide.AddDynamic(this, &UW_ShopCoinManage::CoinFlipAdaptor);
	CoinSubsystem->OnCoinSlotChange.AddDynamic(this, &UW_ShopCoinManage::CoinSlotChangeAdaptor);
	CoinSubsystem->OnSetWeapon.AddDynamic(this, &UW_ShopCoinManage::SetWeaponAdaptor);
	CoinSubsystem->OnHoverWeapon.AddDynamic(this, &UW_ShopCoinManage::ShowDesPanel);
	CoinSubsystem->OnUnHoverWeapon.AddDynamic(this, &UW_ShopCoinManage::HideDesPanel);

	CoinClearButton->OnClicked.AddDynamic(this, &UW_ShopCoinManage::ClearCoin);
	CoinSideClearButton->OnClicked.AddDynamic(this, &UW_ShopCoinManage::ClearCoinSide);

	WeaponDes->SetVisibility(ESlateVisibility::Hidden);
}


void UW_ShopCoinManage::NativeDestruct()
{
    CoinSubsystem->OnChangeSlotCoinSide.RemoveAll(this);
	CoinSubsystem->OnCoinSlotChange.RemoveAll(this);
	CoinSubsystem->OnSetWeapon.RemoveAll(this);
	CoinSubsystem->OnHoverWeapon.RemoveAll(this);
	CoinSubsystem->OnUnHoverWeapon.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_ShopCoinManage::SetDes(int32 WeaponID)
{
    FFaceData FaceData;
    DataSubsystem->TryGetWeapon(WeaponID, FaceData);
	SetDesText(FaceData);
}

void UW_ShopCoinManage::CoinFlipAdaptor(bool IsCoinFront)
{
    FCoinTypeStructure CoinData = CoinSubsystem->GetCurrentSlotCoin();
	if(IsCoinFront)
	{
		SetDes(CoinData.FrontWeaponID);
	}
	else
	{
		SetDes(CoinData.BackWeaponID);
	}

}

void UW_ShopCoinManage::CoinSlotChangeAdaptor()
{
    CoinFlipAdaptor(CoinSubsystem->GetIsCreateCoinFront());
}

void UW_ShopCoinManage::SetWeaponAdaptor(int32 WeaponID)
{
    SetDes(WeaponID);
}


void UW_ShopCoinManage::SetDesText(FFaceData FaceData)
{
    if(FaceData.WeaponID != -1)
		WeaponDes->SetExplainText(FaceData.WeaponName, FaceData.KOR_DES, FaceData.BehaviorPoint, FaceData.AttackPoint);
	else
		WeaponDes->SetExplainTextEmpty();
}
	
void UW_ShopCoinManage::ShowDesPanel(int32 WeaponID)
{
	
	WeaponDes->SetVisibility(ESlateVisibility::Visible);
	SetDes(WeaponID);
}
	
void UW_ShopCoinManage::HideDesPanel()
{
	WeaponDes->SetVisibility(ESlateVisibility::Hidden);
}
	
void UW_ShopCoinManage::ClearCoin()
{
	CoinSubsystem->ResetCoin(CoinSubsystem->GetCurrentSlotNum());
}
	
void UW_ShopCoinManage::ClearCoinSide()
{
	CoinSubsystem->ResetCoinSide(CoinSubsystem->GetCurrentSlotNum(), CoinSubsystem->GetIsCreateCoinFront());
}