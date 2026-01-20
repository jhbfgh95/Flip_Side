// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopSlotActor.h"
#include "Components/BoxComponent.h"
#include "UI/ShopSlotCoinActor.h"
#include "Subsystem/ShopCoinWSubsystem.h"
#include "Components/StaticMeshComponent.h"

AShopSlotActor::AShopSlotActor()
{

    
    BoxCollition = CreateDefaultSubobject<UBoxComponent>(TEXT("SlotCollision"));
    SetRootComponent(BoxCollition);
    SlotMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlotMesh"));
    SlotMesh->SetupAttachment(RootComponent);
}


void AShopSlotActor::BeginPlay()
{
    Super::BeginPlay();

    CoinSubSystem =  GetWorld()->GetSubsystem<UShopCoinWSubsystem>();

    TArray<AActor*> ChildActors;
    GetAllChildActors(ChildActors, false);
    for(int i =0; i< ChildActors.Num(); i++)
    {
        AShopSlotCoinActor* CoinActor= Cast<AShopSlotCoinActor>(ChildActors[i]);
        if(CoinActor)
        {
            SlotCoins.Add(CoinActor);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("%d"), SlotCoins.Num());
}

void AShopSlotActor::AddCoin()
{
    if(CoinSubSystem->CanAddCoin())
    {
        SlotCoins[SlotCoinCount]->AddCoin();
        SlotCoinCount++;
        CoinSubSystem->AddTotalCoin();
    }
}

void AShopSlotActor::RemoveCoin()
{
    if(0<SlotCoinCount)
    {
        SlotCoins[0]->RemoveCoin();
        CoinSubSystem->RemoveTotalCoin();
    }
}