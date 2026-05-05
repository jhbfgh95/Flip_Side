// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardWarning.h"
#include "Subsystem/ShopLevel/ShopCardWSubsystem.h"

void UW_ShopCardWarning::NativeConstruct()
{
    Super::NativeConstruct();
    CardSubsystem = GetWorld()->GetSubsystem<UShopCardWSubsystem>();
    CardSubsystem->OnShopCardWarning.AddDynamic(this,&UW_ShopCardWarning::SetWarning);
    
}

void UW_ShopCardWarning::NativeDestruct()
{
    CardSubsystem->OnShopCardWarning.RemoveAll(this);
    Super::NativeDestruct();
}
	

void UW_ShopCardWarning::SetWarning(int32 WarningCode)
{
    FString text;

    switch (WarningCode)
    {
     case 0:
        text = TEXT("이미 장착한 카드입니다.");
        break;

    case 1:
        text = TEXT("카드가 장착 되어있지 않습니다.");
        break;

    case 2:
        text = TEXT("카드를 더 이상 선택할 수 없습니다.");
        break;
    
    default:
        break;
    }
    ActiveWarning(text);
}


