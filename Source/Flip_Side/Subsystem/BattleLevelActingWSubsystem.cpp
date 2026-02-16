// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/BattleLevelActingWSubsystem.h"
#include "CoinManagementWSubsystem.h"
#include "GridManagerSubsystem.h"
#include "GridTypes.h"
#include "GridActor.h"
#include "CoinActor.h"

void UBattleLevelActingWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CoinManager = Collection.InitializeDependency<UCoinManagementWSubsystem>();
    GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();
}

bool UBattleLevelActingWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    UWorld* World = Cast<UWorld>(Outer);

    if(World)
    {
        FString MapName = World->GetName();
        if(MapName.Contains(TEXT("L_Stage")))
        {
            return true;
        }
    }

    return false;
}

void UBattleLevelActingWSubsystem::WaitTeleportUntilLeverDown()
{
    UWorld* World = GetWorld();
    if (!World) return;

    //tlqkf 레버 타이머 이름이 다 다른 이유는 새벽이라 정신이 없기 때문임
    World->GetTimerManager().ClearTimer(LeverWaitTimer);
    World->GetTimerManager().ClearTimer(LeverDownTimer);
    World->GetTimerManager().ClearTimer(CoinTeleportTimer);

    DoDrawMove.ExecuteIfBound(true);
    World->GetTimerManager().SetTimer(LeverWaitTimer, this, &UBattleLevelActingWSubsystem::MoveCoinsWithDraw, 0.6f, false);
    World->GetTimerManager().SetTimer(LeverDownTimer, this, &UBattleLevelActingWSubsystem::OpenGrid, 1.3f, false);
    World->GetTimerManager().SetTimer(CoinTeleportTimer, this, &UBattleLevelActingWSubsystem::DoCoinTeleportAct, 2.9f, false);
}


void UBattleLevelActingWSubsystem::MoveCoinsWithDraw()
{
    TArray<ACoinActor*> ReadyCoins = CoinManager->GetReadyCoins();

    for (ACoinActor* Coin : ReadyCoins)
    {
        if (IsValid(Coin))
        {
            Coin->DoCoinActAtBattleStartLeverDown();
        }
    }

}

void UBattleLevelActingWSubsystem::OpenGrid()
{
    TArray<ACoinActor*> ReadyCoins = CoinManager->GetReadyCoins();

    for (ACoinActor* Coin : ReadyCoins)
    {
        if (IsValid(Coin))
        {
            GridManager->PlaySingleCellDoorOpenFx(
                Coin->GetDecidedGrid().GridX,
                Coin->GetDecidedGrid().GridY
            );
        }
    }  
}

void UBattleLevelActingWSubsystem::DoCoinTeleportAct()
{
    TArray<ACoinActor*> ReadyCoins = CoinManager->GetReadyCoins();

    for (ACoinActor* Coin : ReadyCoins)
    {
        if (IsValid(Coin))
        {
            TeleportReadyCoinsToDecidedGrid(Coin);
        }
    }
}

void UBattleLevelActingWSubsystem::TeleportReadyCoinsToDecidedGrid(ACoinActor* ReadyCoin)
{
    if(ReadyCoin == nullptr) return;

    if(!ReadyCoin->GetCoinIsReady()) return;

    FGridPoint Grid = ReadyCoin->GetDecidedGrid();

    if(GridManager)
    {
        AGridActor* TheGrid = GridManager->GetGridActor(Grid);
        if(TheGrid)
        {
            ReadyCoin->DoCoinActAtBattleStart(TheGrid->GetGridWorldXY().X, TheGrid->GetGridWorldXY().Y);
        }
    }
}

void UBattleLevelActingWSubsystem::DoSettingAct()
{
    //뭐 또 타이머든 뭐든 써서 n초 후에 세팅 다 하고 다음 턴으로 넘어갈텐데 이제 그동안 연출되는 것들 다 여기에
    DoDrawMove.ExecuteIfBound(false);
}