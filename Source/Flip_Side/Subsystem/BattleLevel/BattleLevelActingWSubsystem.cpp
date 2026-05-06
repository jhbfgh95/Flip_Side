// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/BattleLevel/BattleLevelActingWSubsystem.h"
#include "CoinManagementWSubsystem.h"
#include "GridManagerSubsystem.h"
#include "GridTypes.h"
#include "GridActor.h"
#include "CoinActor.h"
#include "Base_PatternVisualActor.h"

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

    OnCoinLanded.ExecuteIfBound();
}

void UBattleLevelActingWSubsystem::TeleportReadyCoinsToDecidedGrid(ACoinActor* ReadyCoin)
{
    if(ReadyCoin == nullptr) return;

    if(!ReadyCoin->GetCoinOnBattle()) return;

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

void UBattleLevelActingWSubsystem::PrepareBossVisualActor(TSoftClassPtr<ABase_PatternVisualActor> VisualClass)
{
    if (IsValid(CurrentVisualActor))
    {
        CurrentVisualActor->Destroy();
        CurrentVisualActor = nullptr;
    }

    if (VisualClass.IsNull()) return;

    UClass* LoadedClass = VisualClass.LoadSynchronous();
    if (LoadedClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // 위치 파라미터 없이 클래스만으로 스폰
        CurrentVisualActor = GetWorld()->SpawnActor<ABase_PatternVisualActor>(LoadedClass, SpawnParams);
    }
}

void UBattleLevelActingWSubsystem::PlayBossPatternAct()
{
    if (IsValid(CurrentVisualActor))
    {
        CurrentVisualActor->PlayBossPatternAct();
    }
}

void UBattleLevelActingWSubsystem::RaiseCoinForAction(ACoinActor* Coin, FSimpleDelegate OnFinished)
{
    UWorld* World = GetWorld();
    if(!World || !IsValid(Coin))
    {
        OnFinished.ExecuteIfBound();
        return;
    }

    World->GetTimerManager().ClearTimer(CoinActionRaiseTimer);
    World->GetTimerManager().ClearTimer(CoinActionShakeTimer);
    World->GetTimerManager().ClearTimer(CoinActionLowerTimer);

    CoinActionActor = Coin;
    CoinActionRaiseFinished = OnFinished;
    CoinActionElapsedTime = 0.0f;
    CoinActionStartLocation = Coin->GetActorLocation();
    CoinActionTargetLocation = CoinActionStartLocation;
    CoinActionTargetLocation.Z = CoinActionRaiseZ;

    World->GetTimerManager().SetTimer(CoinActionRaiseTimer, this, &UBattleLevelActingWSubsystem::UpdateCoinActionRaise, 0.01f, true);
}

void UBattleLevelActingWSubsystem::UpdateCoinActionRaise()
{
    UWorld* World = GetWorld();
    ACoinActor* Coin = CoinActionActor.Get();
    if(!World || !IsValid(Coin))
    {
        if(World) World->GetTimerManager().ClearTimer(CoinActionRaiseTimer);
        CoinActionRaiseFinished.ExecuteIfBound();
        return;
    }

    CoinActionElapsedTime += 0.01f;
    const float Alpha = FMath::Clamp(CoinActionElapsedTime / FMath::Max(CoinActionRaiseDuration, KINDA_SMALL_NUMBER), 0.0f, 1.0f);
    Coin->SetActorLocation(FMath::Lerp(CoinActionStartLocation, CoinActionTargetLocation, Alpha));

    if(Alpha >= 1.0f)
    {
        World->GetTimerManager().ClearTimer(CoinActionRaiseTimer);
        CoinActionRaiseFinished.ExecuteIfBound();
    }
}

void UBattleLevelActingWSubsystem::ShakeCoinForAction(ACoinActor* Coin, FSimpleDelegate OnFinished)
{
    UWorld* World = GetWorld();
    if(!World || !IsValid(Coin))
    {
        OnFinished.ExecuteIfBound();
        return;
    }

    World->GetTimerManager().ClearTimer(CoinActionShakeTimer);

    CoinActionActor = Coin;
    CoinActionShakeFinished = OnFinished;
    CoinActionElapsedTime = 0.0f;
    CoinActionStartRotation = Coin->GetActorRotation();

    World->GetTimerManager().SetTimer(CoinActionShakeTimer, this, &UBattleLevelActingWSubsystem::UpdateCoinActionShake, 0.01f, true);
}

void UBattleLevelActingWSubsystem::UpdateCoinActionShake()
{
    UWorld* World = GetWorld();
    ACoinActor* Coin = CoinActionActor.Get();
    if(!World || !IsValid(Coin))
    {
        if(World) World->GetTimerManager().ClearTimer(CoinActionShakeTimer);
        CoinActionShakeFinished.ExecuteIfBound();
        return;
    }

    CoinActionElapsedTime += 0.01f;
    const float Alpha = FMath::Clamp(CoinActionElapsedTime / FMath::Max(CoinActionShakeDuration, KINDA_SMALL_NUMBER), 0.0f, 1.0f);
    FRotator NewRotation = CoinActionStartRotation;
    NewRotation.Roll += FMath::Sin(Alpha * 4.0f * PI) * CoinActionShakeRoll;
    Coin->SetActorRotation(NewRotation);

    if(Alpha >= 1.0f)
    {
        Coin->SetActorRotation(CoinActionStartRotation);
        World->GetTimerManager().ClearTimer(CoinActionShakeTimer);
        CoinActionShakeFinished.ExecuteIfBound();
    }
}

void UBattleLevelActingWSubsystem::LowerCoinAfterAction(ACoinActor* Coin, FSimpleDelegate OnFinished)
{
    UWorld* World = GetWorld();
    if(!World || !IsValid(Coin))
    {
        OnFinished.ExecuteIfBound();
        return;
    }

    World->GetTimerManager().ClearTimer(CoinActionLowerTimer);

    CoinActionActor = Coin;
    CoinActionLowerFinished = OnFinished;
    CoinActionElapsedTime = 0.0f;
    CoinActionStartLocation = Coin->GetActorLocation();
    CoinActionTargetLocation = CoinActionStartLocation;
    CoinActionTargetLocation.Z = CoinActionLowerZ;

    World->GetTimerManager().SetTimer(CoinActionLowerTimer, this, &UBattleLevelActingWSubsystem::UpdateCoinActionLower, 0.01f, true);
}

void UBattleLevelActingWSubsystem::UpdateCoinActionLower()
{
    UWorld* World = GetWorld();
    ACoinActor* Coin = CoinActionActor.Get();
    if(!World || !IsValid(Coin))
    {
        if(World) World->GetTimerManager().ClearTimer(CoinActionLowerTimer);
        CoinActionLowerFinished.ExecuteIfBound();
        return;
    }

    CoinActionElapsedTime += 0.01f;
    const float Alpha = FMath::Clamp(CoinActionElapsedTime / FMath::Max(CoinActionLowerDuration, KINDA_SMALL_NUMBER), 0.0f, 1.0f);
    Coin->SetActorLocation(FMath::Lerp(CoinActionStartLocation, CoinActionTargetLocation, Alpha));

    if(Alpha >= 1.0f)
    {
        World->GetTimerManager().ClearTimer(CoinActionLowerTimer);
        CoinActionLowerFinished.ExecuteIfBound();
    }
}
