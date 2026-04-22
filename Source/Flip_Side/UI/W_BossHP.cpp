// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BossHP.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/UniformGridPanel.h"
#include "UI/W_BossHpElement.h"
#include "Components/TextBlock.h"
void UW_BossHP::NativeConstruct()
{
    Super::NativeConstruct();

    if(ClearImage) 
    {
        ClearImage->SetVisibility(ESlateVisibility::Hidden);
    }
    

    for(int i =0; i<HpGrid->GetChildrenCount(); i++)
    {
        HpElements.Add(Cast<UW_BossHpElement>(HpGrid->GetChildAt(i)));
    }
    
}

void UW_BossHP::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

}

void UW_BossHP::InitBossHp(int32 SetMaxHp)
{
    MaxHp = SetMaxHp;
    CurrentHp = SetMaxHp;
    HpText->SetText(FText::AsNumber(CurrentHp));

    int32 RemainderHp = MaxHp % 10;
    int32 DividsHp = MaxHp / 10;
    for(int i =0; i<HpElements.Num(); i++)
    {
        if(i<RemainderHp)
        {
            HpElements[i]->InitElementHp(DividsHp+1);
        }
        else
            HpElements[i]->InitElementHp(DividsHp);
    }
}

void UW_BossHP::InitBossShield(int32 SetMaxShield)
{
    MaxShield = SetMaxShield;
    CurrentShield = SetMaxShield;

    int32 RemainderShield = MaxShield % 10;
    int32 DividsShield = MaxShield / 10;
    for(int i =0; i<HpElements.Num(); i++)
    {
        if(i<RemainderShield)
        {
            HpElements[i]->InitElementShield(DividsShield+1);
        }
        else
            HpElements[i]->InitElementShield(DividsShield);
    }

    ShieldText->SetText(FText::AsNumber(CurrentShield));

}

void UW_BossHP::ChangeMaxHp(int32 AddMaxHp)
{
    
    if(MaxHp + AddMaxHp < CurrentHp)
    {
        ChangeCurrentHp(AddMaxHp);
        return;
    }

    MaxHp += AddMaxHp;
    HpText->SetText(FText::AsNumber(CurrentHp));
}

void UW_BossHP::ChangeCurrentHp(int32 AddHpValue)
{

    //HpElements[GetCurrentHpElemetNum()]
    if(0<=AddHpValue)
    {
        for(int i =0; i< AddHpValue; i++)
        {
            CurrentHp++;
            HpElements[GetCurrentHpElemetNum()]->IncreaseHp();
        }
    }
    else
    {
        int32 ChangeValue = FMath::Abs(AddHpValue);
        for(int i =0; i< ChangeValue; i++)
        {
            HpElements[GetCurrentHpElemetNum()]->DecreaseHp();
            CurrentHp--;
        }
    }
    
    HpText->SetText(FText::AsNumber(CurrentHp));

}

	
void UW_BossHP::ChangeMaxShield(int32 AddMaxShield)
{
    if(MaxShield + AddMaxShield < CurrentShield)
    {
        ChangeCurrentShield(AddMaxShield);
        return;
    }

    MaxShield += AddMaxShield;

    ShieldText->SetText(FText::AsNumber(CurrentShield));
}

void UW_BossHP::ChangeCurrentShield(int32 AddShieldValue)
{
    UE_LOG(LogTemp, Warning, TEXT("보호막 변동값%d"), AddShieldValue);
    if(0<=AddShieldValue)
    {
        for(int i =0; i< AddShieldValue; i++)
        {
            CurrentShield++;
            HpElements[GetCurrentShieldElemetNum()]->IncreaseShield();
        }
    }
    else
    {
        int32 ChangeValue = FMath::Abs(AddShieldValue);
        for(int i =0; i< ChangeValue; i++)
        {
            
            HpElements[GetCurrentShieldElemetNum()]->DecreaseShield();
            CurrentShield--;
        }
    }
    
    ShieldText->SetText(FText::AsNumber(CurrentShield));
}



void UW_BossHP::ShowClearImage()
{
    if (ClearImage)
    {
        // 이미지를 화면에 보여줍니다 (클릭 이벤트를 무시하려면 HitTestInvisible 추천)
        ClearImage->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}
	
int UW_BossHP::GetCurrentHpElemetNum()
{
    int Num = (CurrentHp % HpElements.Num()) - 1;
    if (Num <=-1)
        return 9;
    else 
        return Num;
}
	
int UW_BossHP::GetCurrentShieldElemetNum()
{
        int Num = (CurrentShield % HpElements.Num()) - 1;
    if (Num <=-1)
        return 9;
    else 
        return Num;
}