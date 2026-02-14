// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/BattleManagerWSubsystem.h"
#include "CoinActor.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.h"
#include "LevelGISubsystem.h"
#include "CoinManagementWSubsystem.h"
#include "CrossingLevelGISubsystem.h"
#include "GridManagerSubsystem.h"
#include "TemplateFunction_Utils.h"

#define BATTLE_COIN_MAX 10

void UBattleManagerWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    CoinOrderArray.SetNum(BATTLE_COIN_MAX);

    CoinOrderArrayInit();
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
            UE_LOG(LogTemp, Warning, TEXT("BattleManagerOn"));
            return true;
        }
    }

    return false;
}

//다 -1로 세팅 후 초기화
//이후에 예외 검사할 때 전부 -1이면 return판정
void UBattleManagerWSubsystem::CoinOrderArrayInit()
{
    for(int i = 0; i < BATTLE_COIN_MAX; i++)
    {
        CoinOrderArray[i].CoinID = -1;
        CoinOrderArray[i].CoinGrid.GridX = -1;
        CoinOrderArray[i].CoinGrid.GridY = -1;
        CoinOrderArray[i].SelectedWeaponID = -1;
    }
}

void UBattleManagerWSubsystem::TurnStackInit()
{
    TurnManageMentStack.Push(ETurnState::SettingTurn);
    TurnManageMentStack.Push(ETurnState::BossTurn);
    TurnManageMentStack.Push(ETurnState::BehaviorTurn);
    TurnManageMentStack.Push(ETurnState::CoinSelectTurn);
    TurnManageMentStack.Push(ETurnState::CoinReadyTurn);

    CurrentTurn = TurnManageMentStack.Top();
}

ETurnState UBattleManagerWSubsystem::GetCurrentTurn()
{
    return TurnManageMentStack.Top();
}

void UBattleManagerWSubsystem::StartBattleFromLever() { TurnProgressing(); }

void UBattleManagerWSubsystem::TurnProgressing()
{
    // 이전 턴 상태 보관 (로그용)
    ETurnState PreviousTurn = CurrentTurn;

    // 코인 레디턴 -> 코인 설렉트턴 -> 비헤이비어 턴 -> 보스 턴 -> 세팅 턴
    TurnManageMentStack.Pop();
    CurrentTurn = TurnManageMentStack.Top();

    OnTurnChanged.Broadcast(CurrentTurn);

    // 로그 출력: 어떤 턴에서 어떤 턴으로 넘어갔는지 명시
    const UEnum* EnumPtr = StaticEnum<ETurnState>();
    FString PrevName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)PreviousTurn) : TEXT("None");
    FString NextName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)CurrentTurn) : TEXT("None");

    UE_LOG(LogTemp, Warning, TEXT("#### [턴 전환] %s ===> %s ####"), *PrevName, *NextName);

    switch(CurrentTurn)
    {
    case ETurnState::CoinReadyTurn:
        break;
        
    case ETurnState::CoinSelectTurn:
        MatchCoinsToRandomState();
        break;
    
    case ETurnState::BehaviorTurn:
        break;
    
    case ETurnState::BossTurn:
        break;
    case ETurnState::SettingTurn:
        CoinOrderArrayInit();
        TurnStackInit();
        GenerateRandomStates();
        break;
    }
}

void UBattleManagerWSubsystem::GenerateRandomStates()
{
    UGridManagerSubsystem* GridManager = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
    if(!GridManager) return;

    if(GridManager->GridXSize <= 0 || GridManager->GridYSize <= 0) return;

    for(int i =0; i<BATTLE_COIN_MAX; i++)
    {
        EFaceState DecidedFace = UTemplateFunction_Utils::GetRandomENum<EFaceState>();
        int32 RandomX = FMath::RandRange(0, GridManager->GridXSize);
        int32 RandomY = FMath::RandRange(0, GridManager->GridYSize);

        RandomStateArray[i].RandomFace = DecidedFace;
        RandomStateArray[i].RandomGrid.GridX = RandomX;
        RandomStateArray[i].RandomGrid.GridY = RandomY;
    }
}

void UBattleManagerWSubsystem::MatchCoinsToRandomState()
{
    UCoinManagementWSubsystem* CoinManager = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
    if(!CoinManager) return;    

    TArray<ACoinActor*> ReadyCoins = CoinManager->GetReadyCoins();

    for(int i = 0; i<CoinManager->GetBattleReadyCoinNum(); i++)
    {
        ReadyCoins[i]->SetCoinFace(RandomStateArray[i].RandomFace);
        ReadyCoins[i]->SetGridPoint(RandomStateArray[i].RandomGrid);
        //UBehavior 실행할 때 이거에서 빼오려고 쓴거긴 함.
        MatchedArray.Add(ReadyCoins[i]->GetCoinID(), ReadyCoins[i]->GetCoinFaceID());
    }
}