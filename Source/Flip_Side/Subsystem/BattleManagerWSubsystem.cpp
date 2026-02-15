// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/BattleManagerWSubsystem.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "FlipSide_Enum.h"
#include "CoinDataTypes.h"
#include "LevelGISubsystem.h"
#include "CoinManagementWSubsystem.h"
#include "CrossingLevelGISubsystem.h"
#include "GridManagerSubsystem.h"
#include "BattleLevelActingWSubsystem.h"
#include "TemplateFunction_Utils.h"

#define BATTLE_COIN_MAX 10

void UBattleManagerWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CoinOrderArray.SetNum(BATTLE_COIN_MAX);

    CoinManager = Collection.InitializeDependency<UCoinManagementWSubsystem>();
    GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();
    ActingManager = Collection.InitializeDependency<UBattleLevelActingWSubsystem>();

    RandomStateArray.SetNum(BATTLE_COIN_MAX);
    CoinOrderArray.SetNum(BATTLE_COIN_MAX);

    CoinOrderArrayInit();
    GenerateRandomStates();
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

            if (!SelectedPoints.Contains(NewPoint))
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
    if(!CoinManager) return;

    MatchedArray.Empty();

    TArray<ACoinActor*> ReadyCoins = CoinManager->GetReadyCoins();

    int32 StateIndex = 0;
    for (ACoinActor* Coin : ReadyCoins)
    {
        if (IsValid(Coin))
        {
            Coin->SetCoinFace(RandomStateArray[StateIndex].RandomFace);
            Coin->SetGridPoint(RandomStateArray[StateIndex].RandomGrid);
            
            MatchedArray.Add(Coin->GetCoinID(), Coin->GetCoinFaceID());
            StateIndex++; // 코인을 찾았을 때만 다음 랜덤 상태로
        }
    }

    ActingManager->WaitTeleportUntilLeverDown();
}


void UBattleManagerWSubsystem::AddCoinsToOrderArray(ACoinActor* TargetCoin)
{
    //진수가 하기로 함ㅇㅇ; -> 클릭하면 FBattleCoinInfo를 TargetCoin의 그거로 채워주는 거임ㅇㅇ;
    //만약에 저거 그냥 FStruct로 모자랄거 같으면 걍 말하셈 근데 아마 내 설계대로면 딱히 문제없을거임ㅇㅇ
}