// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "UseableItemActor.h"
#include "Component_Status.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.h"
#include "LevelGISubsystem.h"
#include "CoinManagementWSubsystem.h"
#include "BossManagerSubsystem.h"
#include "BossSetupGISubsystem.h"
#include "CrossingLevelGISubsystem.h"
#include "UseableItemWSubsystem.h"
#include "GridManagerSubsystem.h"
#include "BattleLevelActingWSubsystem.h"
#include "CoinActionManagementWSubsystem.h"
#include "TemplateFunction_Utils.h"

#define BATTLE_COIN_MAX 10

void UBattleManagerWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CoinManager = Collection.InitializeDependency<UCoinManagementWSubsystem>();
    GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();
    ItemManager = Collection.InitializeDependency<UUseableItemWSubsystem>();
    ActingManager = Collection.InitializeDependency<UBattleLevelActingWSubsystem>();
    CoinActionManager = Collection.InitializeDependency<UCoinActionManagementWSubsystem>();
    BossManager = Collection.InitializeDependency<UBossManagerSubsystem>();

    RandomStateArray.SetNum(BATTLE_COIN_MAX);

    TurnStackInit();
}

bool UBattleManagerWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UBattleManagerWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    UGameInstance* GI = InWorld.GetGameInstance();
    if (GI)
    {
        ULevelGISubsystem* LevelGI = GI->GetSubsystem<ULevelGISubsystem>();
        UBossSetupGISubsystem* BossSetupGI = GI->GetSubsystem<UBossSetupGISubsystem>();

        if (LevelGI && BossSetupGI)
        {
            const int32 StageIndex = LevelGI->GetBattleLevelIndex();
            BossSetupGI->PrepareBossForStage(StageIndex);
        }
    }

    if (BossManager)
    {
        if (BossManager->SpawnPreparedBoss())
        {
            if (ABossActor* Boss = BossManager->GetCurrentBoss())
            {
                Boss->OnBossAttackEnded.AddDynamic(this, &UBattleManagerWSubsystem::DoSettingTurn);
            }
        }
    }
    DoSettingTurn();
}

void UBattleManagerWSubsystem::TurnStackInit()
{
    TurnManageMentStack.Push(ETurnState::BossTurn);
    TurnManageMentStack.Push(ETurnState::CoinSelectTurn);
    TurnManageMentStack.Push(ETurnState::CoinReadyTurn);
    TurnManageMentStack.Push(ETurnState::SettingTurn);

    CurrentTurn = TurnManageMentStack.Top();
}

ETurnState UBattleManagerWSubsystem::GetCurrentTurn()
{
    return TurnManageMentStack.Top();
}

void UBattleManagerWSubsystem::StartBattleFromLever() { TurnProgressing(); }

void UBattleManagerWSubsystem::TurnProgressing()
{
    //세팅 턴 -> 코인 레디턴 -> 코인 설렉트턴 -> 보스 턴 
    TurnManageMentStack.Pop();
    CurrentTurn = TurnManageMentStack.Top();

    OnTurnChanged.Broadcast(CurrentTurn);

    switch(CurrentTurn)
    {
    case ETurnState::SettingTurn:
        DoSettingTurn();
        break;
    case ETurnState::CoinReadyTurn:
        DoCoinReadyTurn();
        break;
    case ETurnState::CoinSelectTurn:
        DoCoinSelectTurn();
        break;
    case ETurnState::BossTurn:
        DoBossTurn();
        break;
    }

}

void UBattleManagerWSubsystem::GenerateRandomStates()
{
    if(!GridManager) return;

    if(GridManager->GridXSize <= 0 || GridManager->GridYSize <= 0) return;

    TSet<FGridPoint> SelectedPoints;
    for(int i =0; i<BATTLE_COIN_MAX; i++)
    {
        FGridPoint NewPoint;
        bool bIsUnique = false;

        EFaceState DecidedFace = UTemplateFunction_Utils::GetRandomENum<EFaceState>();

        //중복 좌표 예외처리
        while (!bIsUnique)
        {
            NewPoint.GridX = FMath::RandRange(0, GridManager->GridXSize - 1);
            NewPoint.GridY = FMath::RandRange(0, GridManager->GridYSize - 1);

            if (!SelectedPoints.Contains(NewPoint) && !GridManager->GetGridActor(NewPoint)->GetIsOccupied())
            {
                SelectedPoints.Add(NewPoint);
                bIsUnique = true;
            }
        }

        RandomStateArray[i].RandomFace = DecidedFace;
        RandomStateArray[i].RandomGrid = NewPoint;
    }
}

void UBattleManagerWSubsystem::MatchCoinsToRandomState()
{
    if(!CoinManager || !GridManager) return;

    TArray<ACoinActor*> ReadyCoins = CoinManager->GetReadyCoins();

    int32 StateIndex = 0;
    for (ACoinActor* Coin : ReadyCoins)
    {
        if (IsValid(Coin))
        {
            CoinManager->LockCoinReady(Coin);
            Coin->SetCoinFace(RandomStateArray[StateIndex].RandomFace);
            Coin->SetGridPoint(RandomStateArray[StateIndex].RandomGrid);
            Coin->SetCoinOnBattle(true);
            Coin->SetCoinIsReady(false);

            GridManager->GetGridActor(RandomStateArray[StateIndex].RandomGrid)->SetOccupied(true, EGridOccupyingType::Coin, Coin);
            
            StateIndex++; // 코인을 찾았을 때만 다음 랜덤 상태로
        }
    }

    ActingManager->WaitTeleportUntilLeverDown();
}

void UBattleManagerWSubsystem::DoCoinReadyTurn()
{
    UE_LOG(LogTemp, Warning, TEXT("ReadyTurn"));
}

void UBattleManagerWSubsystem::DoCoinSelectTurn()
{
    MatchCoinsToRandomState();
    CoinActionManager->SetTurn(true);
    ItemManager->SetTurn(true);
}


void UBattleManagerWSubsystem::DoBossTurn()
{
    CoinActionManager->SetTurn(false);
    ItemManager->SetTurn(false);
    BossManager->ExecuteCurrentPattern();
    TurnStackInit();
}

void UBattleManagerWSubsystem::DoSettingTurn()
{
    GenerateRandomStates();
    BossManager->StartBossSetting();
    ActingManager->DoSettingAct();
    GridManager->InitCoinOccupied();
    CoinManager->CheckBattleReadyCoinAlive();

    TurnProgressing();
}