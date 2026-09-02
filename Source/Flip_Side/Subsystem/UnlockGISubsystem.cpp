// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/UnlockGISubsystem.h"
#include "Kismet/GameplayStatics.h"

void UUnlockGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UUnlockGISubsystem::OnLevelLoad);
    ResetUnlockData();
}

void UUnlockGISubsystem::Deinitialize()
{
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
    Super::Deinitialize();
}

void UUnlockGISubsystem::OnLevelLoad(UWorld* LoadedWorld)
{
    if (!LoadedWorld) return;
    if (!LoadedWorld->IsGameWorld()) return;

    FString LevelName = UGameplayStatics::GetCurrentLevelName(LoadedWorld, true);
    if(LevelName.Equals(TEXT("L_GameStart")))
    {
        ResetUnlockData();
    }
}


void UUnlockGISubsystem::ResetUnlockData()
{
	CardUnlockArray.Reset();
    
    if(UnlockWeaponArray.Num()<=0)
    {
        UnlockWeaponArray.Add(1);
    }
}

void UUnlockGISubsystem::UnlockWeapon(int32 ID)
{
    UnlockWeaponArray.Add(ID);
    OnWeaponUnlock.Broadcast(ID);
}

void UUnlockGISubsystem::UnlockCard(int32 ID)
{
    if(CardUnlockArray.Find(ID) == -1)
    {
        CardUnlockArray.Add(ID);
        OnUnlockCard.Broadcast(ID);
    }
        
}


int32 UUnlockGISubsystem::GetUnlockWeaponArrrayNum()
{
    return UnlockWeaponArray.Num();
}

int32 UUnlockGISubsystem::GetUnlockCardArrayNum()
{
    return CardUnlockArray.Num();
}

bool UUnlockGISubsystem::IsCardUnlockByID(int32 ID)
{
    if(CardUnlockArray.Find(ID) ==-1)
            return false;
        else
            return true;
}

bool UUnlockGISubsystem::IsWeaponUnlockByID(int32 ID)
{
    return UnlockWeaponArray.Contains(ID);
}

const TArray<int32>& UUnlockGISubsystem::GetUnlockWeaponArray()
{
    return UnlockWeaponArray;
}
	
const TArray<int32>& UUnlockGISubsystem::GetUnlockCardArray()
{
    return CardUnlockArray;
}