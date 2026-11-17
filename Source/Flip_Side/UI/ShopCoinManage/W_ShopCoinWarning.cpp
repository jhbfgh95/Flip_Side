// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinWarning.h"
#include "Engine/World.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
void UW_ShopCoinWarning::NativeConstruct()
{
    Super::NativeConstruct();
}

	

void UW_ShopCoinWarning::SetWarning(int32 WarningCode)
{
    FText text;

    switch (WarningCode)
    {
     case 0:
        text = NSLOCTEXT("W_ShopCoinWarning", "Warning_0", "양면에 같은 무기를 장착할 수 없습니다.");
        break;

    case 1:
        text = NSLOCTEXT("W_ShopCoinWarning", "Warning_1", "이미 같은 무기가 장착된 코인이 존재합니다.");
        break;

    case 2:
        text = NSLOCTEXT("W_ShopCoinWarning", "Warning_2", "양면에 무기가 장착되어 있지 않습니다.");
        break;

    case 3:
        text = NSLOCTEXT("W_ShopCoinWarning", "Warning_3", "최대 30개의 코인만 보유할 수 있습니다.");
        break;

    case 4:
        text = NSLOCTEXT("W_ShopCoinWarning", "Warning_4", "최대 10개의 코인슬롯만 보유할 수 있습니다.");
        break;
    
    
    default:
        return;
    }
    ActiveWarning(text, false);

    if (UWorld* World = GetWorld())
    {
        if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
        {
            SoundManager->PlayWarningSound();
        }
    }
}
