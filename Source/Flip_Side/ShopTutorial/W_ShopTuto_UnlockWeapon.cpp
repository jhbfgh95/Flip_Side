// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_ShopTuto_UnlockWeapon.h"
#include "Components/Button.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponGrid.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "UI/W_WeaponDescription.h"
#include "UI/W_PriceWidget.h"
void UW_ShopTuto_UnlockWeapon::NativeConstruct()
{
    Super::NativeConstruct();
    
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
    DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    UnlockWeaponSubsystem->OnSelectUnlockWeapon.AddDynamic(this, &UW_ShopTuto_UnlockWeapon::SetWidgetState);
    HideWidget();
}

void UW_ShopTuto_UnlockWeapon::NativeDestruct()
{
    UnlockWeaponSubsystem->OnSelectUnlockWeapon.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_ShopTuto_UnlockWeapon::SetWidgetState(EWeaponClass WeaponClass, int32 WeaponID, bool IsItemUnlock)
{
    if(IsItemUnlock)
    {
        HideWidget();
    }   
    else
    {
        VisibleWidget();
    }
    SetDes(WeaponID);
}
	
void UW_ShopTuto_UnlockWeapon::HideWidget()
{
    UnlockButton->SetVisibility(ESlateVisibility::Collapsed);
    WeaponPriceText->SetVisibility(ESlateVisibility::Collapsed);
    
    FString ExpainText = TEXT("구매할 무기를 선택하세요");
	WeaponDes->SetPanelStringText(ExpainText, TEXT(""));
}
	
void UW_ShopTuto_UnlockWeapon::VisibleWidget()
{
    UnlockButton->SetVisibility(ESlateVisibility::Visible);
    WeaponPriceText->SetVisibility(ESlateVisibility::Visible);
}


void UW_ShopTuto_UnlockWeapon::SetDes(int32 WeaponID)
{
    FFaceData FaceData;
    DataSubsystem->TryGetWeapon(WeaponID, FaceData);
	SetDesText(FaceData);
}


void UW_ShopTuto_UnlockWeapon::SetDesText(FFaceData FaceData)
{
    if(FaceData.WeaponID != -1)
		WeaponDes->SetExplainText(FaceData.WeaponName, FaceData.KOR_DES, FaceData.BehaviorPoint, FaceData.AttackPoint);
	else
    {
        FString ExpainText = TEXT("구매할 무기를 선택하세요");
		WeaponDes->SetPanelStringText(ExpainText, TEXT(""));
    }
    WeaponPriceText->SetPriceText(FaceData.Price);
}

