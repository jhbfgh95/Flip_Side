// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponWidget.h"
#include "Components/Button.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/W_WeaponDescription.h"
#include "UI/W_PriceWidget.h"
void UW_UnlockWeaponWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    UnlockWeaponSubsystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
    DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    //UnlockWeaponSubsystem->OnSelectUnlockWeapon.AddDynamic(this, &UW_UnlockWeaponWidget::SetWidgetState);
    HideWidget();
}

void UW_UnlockWeaponWidget::NativeDestruct()
{
    UnlockWeaponSubsystem->OnSelectUnlockWeapon.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_UnlockWeaponWidget::SetWidgetState(EWeaponClass WeaponClass, int32 WeaponID, bool IsItemUnlock)
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
	
void UW_UnlockWeaponWidget::HideWidget()
{
    UnlockButton->SetVisibility(ESlateVisibility::Collapsed);
    WeaponPriceText->SetVisibility(ESlateVisibility::Collapsed);
    
    FString ExpainText = TEXT("구매할 무기를 선택하세요");
	WeaponDes->SetPanelStringText(ExpainText, TEXT(""));
}
	
void UW_UnlockWeaponWidget::VisibleWidget()
{
    UnlockButton->SetVisibility(ESlateVisibility::Visible);
    WeaponPriceText->SetVisibility(ESlateVisibility::Visible);
}


void UW_UnlockWeaponWidget::SetDes(int32 WeaponID)
{
    FFaceData FaceData;
    DataSubsystem->TryGetWeapon(WeaponID, FaceData);
	SetDesText(FaceData);
}


void UW_UnlockWeaponWidget::SetDesText(FFaceData FaceData)
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