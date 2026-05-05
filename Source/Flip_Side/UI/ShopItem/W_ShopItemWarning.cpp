// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemWarning.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"

void UW_ShopItemWarning::NativeConstruct()
{
    Super::NativeConstruct();

    ItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    ItemSubsystem->OnShopItemWarning.AddDynamic(this,&UW_ShopItemWarning::SetWarning);
    
}

void UW_ShopItemWarning::NativeDestruct()
{
    ItemSubsystem->OnShopItemWarning.RemoveAll(this);
    Super::NativeDestruct();
}
	

void UW_ShopItemWarning::SetWarning(int32 WarningCode)
{
    FString text;

    switch (WarningCode)
    {
     case 0:
        text = TEXT("돈이 부족합니다.");
        break;
    
    default:
        break;
    }
    ActiveWarning(text);
}



