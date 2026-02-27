// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CoinCreateWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"

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


void UCoinCreateWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UCoinCreateWSubsystem::OnWorldBeginPlay(UWorld& World)
{
    Super::OnWorldBeginPlay(World);

    
}

//코인의 앞뒤를 변경
void UCoinCreateWSubsystem::ChangeCoinSide()
{
    if(bIsCreateCoinFront)
    {
        bIsCreateCoinFront = false;
    }
    else
    {
        bIsCreateCoinFront = true;
    }
    UE_LOG(LogTemp, Warning, TEXT("무기 앞면 인가? : %d"), bIsCreateCoinFront);

}

FCoinTypeStructure UCoinCreateWSubsystem::GetSelectCoin()
{
    return SelectedCoin;
}

//코인 선택
void UCoinCreateWSubsystem::SelectCoin(FCoinTypeStructure SelectCoinInfo, EWeaponClass CoinClass)
{
    SelectedCoin = SelectCoinInfo;
    SetCoinClass(CoinClass);
    bIsCreateCoinFront = true;

    OnSelectedCoin.Broadcast(SelectedCoin, CoinClass);
    OnCoinClassUpdate.Broadcast(CoinClass);
}

//선택된 코인 면의 ID를 변경
void UCoinCreateWSubsystem::ChangeSelectedCoinWeapon(int32 WeaponIndex)
{
    if(bIsCreateCoinFront)
    {
        SelectedCoin.FrontWeaponID = WeaponIndex;
    }
    else
    {
        SelectedCoin.BackWeaponID = WeaponIndex;
    }
    OnSelectedCoinUpdate.Broadcast(WeaponIndex);
}


void UCoinCreateWSubsystem::SetCoinClass(EWeaponClass weponClass)
{
    if(SelectedCoinClass != weponClass)
    {
        SelectedCoinClass = weponClass;
        
        //SelectedCoin.FrontWeaponID = -1;
        //SelectedCoin.BackWeaponID = -1;
    }
    OnCoinClassUpdate.Broadcast(weponClass);
}


bool UCoinCreateWSubsystem::GetIsCreateCoinFront()
{
    return bIsCreateCoinFront;
}


EWeaponClass UCoinCreateWSubsystem::GetSelectCoinClass()
{
    return SelectedCoinClass;
}
