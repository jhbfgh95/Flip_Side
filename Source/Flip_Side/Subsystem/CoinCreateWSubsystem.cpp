// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CoinCreateWSubsystem.h"
bool UCoinCreateWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    Super::ShouldCreateSubsystem(Outer);

    UWorld* World = Cast<UWorld>(Outer);

    if (!World || !World->IsGameWorld())
    {
        return false;
    }

    const FString MapName = World->GetMapName();
    return MapName.Contains(TEXT("L_ShopLevel"));
} 


//코인의 앞뒤를 변경
void UCoinCreateWSubsystem::ChangeCoinSide()
{
    if(bIsCreateCoinFront)
    {
        bIsCreateCoinFront = false;
        OnSelectedCoinUpdate.Broadcast(SelectedCoin.FrontWeaponID);
    }
    else
    {
        bIsCreateCoinFront = true;
        OnSelectedCoinUpdate.Broadcast(SelectedCoin.BackWeaponID);
    }
        

}


FCoinTypeStructure UCoinCreateWSubsystem::GetSelectCoin()
{
    return SelectedCoin;
}
//코인 선택
void UCoinCreateWSubsystem::SelectCoin(FCoinTypeStructure SelectCoinInfo)
{
    SelectedCoin = SelectCoinInfo;
    
    OnSelectedCoinChanged.Broadcast(SelectedCoin);
}

//선택된 코인 면의 ID를 변경
void UCoinCreateWSubsystem::ChangeSelectedCoinWeapon(int32 WeaponID)
{
    if(bIsCreateCoinFront)
    {
        SelectedCoin.FrontWeaponID = WeaponID;
    }
    else
    {
        SelectedCoin.BackWeaponID = WeaponID;
    }
    OnSelectedCoinUpdate.Broadcast(WeaponID);
}


void UCoinCreateWSubsystem::SetCoinClass(EWeaponClass weponClass)
{
    UE_LOG(LogTemp, Warning, TEXT("클래스설정"));
    OnCoinClassUpdate.Broadcast(weponClass);
}


bool UCoinCreateWSubsystem::GetIsCreateCoinFront()
{
    return bIsCreateCoinFront;
}