// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinWarning.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
void UW_ShopCoinWarning::NativeConstruct()
{
    Super::NativeConstruct();
    CoinSubsystem = GetWorld()->GetSubsystem<UShopCoinWSubsystem>();
    CoinSubsystem->OnWarningCreate.AddDynamic(this,&UW_ShopCoinWarning::ActiveWarning);
    
}

void UW_ShopCoinWarning::NativeDestruct()
{
    CoinSubsystem->OnWarningCreate.RemoveAll(this);
    Super::NativeDestruct();
}
	

void UW_ShopCoinWarning::ActiveWarning(int32 WarningCode)
{
    FString text;
    LockWarningImage->SetVisibility(ESlateVisibility::Collapsed);

    WarningImage->SetVisibility(ESlateVisibility::Collapsed);

    switch (WarningCode)
    {
     case 0:
        text = TEXT("양면에 같은 무기를 장착할 수 없습니다.");
        WarningImage->SetVisibility(ESlateVisibility::Visible);
        break;

    case 1:
        text = TEXT("이미 같은 동전이 존재합니다.");
        WarningImage->SetVisibility(ESlateVisibility::Visible);
        break;

    case 2:
        text = "슬롯이 잠겨있습니다.";
        LockWarningImage->SetVisibility(ESlateVisibility::Visible);
        break;
        
    case 3:
        text = TEXT("양면에 무기가 장착되어 있지 않습니다.");
        WarningImage->SetVisibility(ESlateVisibility::Visible);
        break;
    
    default:
        break;
    }
    
    WarningText->SetText(FText::FromString(text));
    PlayAnimation(WarningTextAnim);
}
