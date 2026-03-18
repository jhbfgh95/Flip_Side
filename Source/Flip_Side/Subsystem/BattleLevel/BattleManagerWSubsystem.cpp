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

    if(BossManager)
    {
        BossManager->SpawnBossForStage();
        BossManager->GetCurrentBoss()->OnBossAttackEnded.AddDynamic(this, &UBattleManagerWSubsystem::DoSettingTurn);
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
    if(!CoinManager || !GridManager) return;

    TArray<ACoinActor*> ReadyCoins = CoinManager->GetReadyCoins();

    int32 StateIndex = 0;
    for (ACoinActor* Coin : ReadyCoins)
    {
        if (IsValid(Coin))
        {
            Coin->SetCoinFace(RandomStateArray[StateIndex].RandomFace);
            Coin->SetGridPoint(RandomStateArray[StateIndex].RandomGrid);

            GridManager->GetGridActor(RandomStateArray[StateIndex].RandomGrid)->SetOccupied(true, EGridOccupyingType::Coin, Coin);
            
            StateIndex++; // 코인을 찾았을 때만 다음 랜덤 상태로
        }
    }

    ActingManager->WaitTeleportUntilLeverDown();
}

void UBattleManagerWSubsystem::HandleItemClicked(AUseableItemActor* TargetItem)
{
    if(!TargetItem) return;

    bItemFlag = true;
    // CoinActionManager->CurrentInputState = EActionInputState::WaitingForGridClickForItem;
    // ItemManager->SelectWantUseItem(TargetItem);

    // 상태 매니저의 상태를 '아이템 사용을 위한 그리드 클릭 대기'로 변경
    if (CoinActionManager)
    {
        CoinActionManager->CurrentInputState = EActionInputState::WaitingForGridClickForItem;
    }

    // 아이템 매니저에게 어떤 아이템이 선택됐는지 알림
    if (ItemManager)
    {
        ItemManager->SelectWantUseItem(TargetItem);
    }
}

void UBattleManagerWSubsystem::HandleGridClicked(AGridActor* TargetGrid)
{
    if(!TargetGrid) return;
    /*
    UE_LOG(LogTemp, Warning, TEXT("#### BM: Clicked %s | Type: %d | Flag: %s | State: %d ####"),
           *TargetGrid->GetName(), (int32)TargetGrid->GetCurrentOccupyingThing(),
           bItemFlag ? TEXT("TRUE") : TEXT("FALSE"), (int32)CoinActionManager->CurrentInputState);
    */
    if (CoinActionManager->CurrentInputState == EActionInputState::ExecutingAction)
    {
        return; 
    }

    if(TargetGrid->GetCurrentOccupyingThing() == EGridOccupyingType::Coin)
    {
        if(CoinActionManager->CurrentInputState == EActionInputState::WaitingForGridClick)
        {
            //2. 먄약 한 칸 선택이면 그냥 그리드로 보면 되니까 그리드로 좌표빼냄
            CoinActionManager->ExecuteTimeAction(TargetGrid->GetGridPoint());
        }
        else if (CoinActionManager->CurrentInputState == EActionInputState::None)
        {
            //1. 여기는 처음에 코인 봐야해서 코인에서 좌표를 뻈고
            
            ACoinActor* Coin = Cast<ACoinActor>(TargetGrid->GetCurrentOccupied());
            CoinActionManager->SetSelectedWeapon(Coin->StatComponent->GetModifiedStats(Coin->GetCoinFaceID()), Coin->GetDecidedGrid());                
            CoinActionManager->SetCasterCoin(Coin);
        }
        else if(CoinActionManager->CurrentInputState == EActionInputState::WaitingForGridClickForItem && bItemFlag)
        {
            // [로그 1] 여기까지 진입했는지 확인
            UE_LOG(LogTemp, Warning, TEXT("#### BM: Grid Clicked for Item - Calling ExecuteItem ####"));

            ACoinActor *Coin = Cast<ACoinActor>(TargetGrid->GetCurrentOccupied());
            ItemManager->ExecuteItem(TargetGrid);
            CoinActionManager->CurrentInputState = EActionInputState::None;
            bItemFlag = false;

            // [로그 2] 실행 후 상태가 정상적으로 풀리는지 확인
            UE_LOG(LogTemp, Warning, TEXT("#### BM: Item Use Flow Finished - State Reset ####"));
        }
    }
    else if(TargetGrid->GetCurrentOccupyingThing() == EGridOccupyingType::None)
    {
        if(bItemFlag&&ItemManager->IsItemSetupInGrid())
        {
            ItemManager->ExecuteItem(TargetGrid);
            CoinActionManager->CurrentInputState = EActionInputState::None;
            bItemFlag = false;
        }
        else
        {
            //몰?루 아마 좆까세요~ 못해요~ 이런 거 표시해야겠지?
        }
    }
    
}

//한 가지 말을 해보자면, 캔슬 코드에는 그냥 데이터만 Init하는 함수들만 불러놨음
//그러다보니까 선택한 액터를 캐싱을 안해뒀는데, 필요하면 추가해서 쓰셈 또한, 인풋은 절대로 BattleManager에서만 바인딩 했으면 좋겠음.
//다른 매니저들을 수정해도 상관은 없지만, 인풋 직접바인딩은 배틀매니저에서만 부탁함.
void UBattleManagerWSubsystem::HandleGridCanceled()
{
    CoinActionManager->CancelSelectWeapon();
}

void UBattleManagerWSubsystem::HandleItemCanceled()
{
    ItemManager->CancelWantUseItem();
    CoinActionManager->CurrentInputState = EActionInputState::None;
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
    CoinActionManager->SetTurn(false);
    ItemManager->SetTurn(false);
    ActingManager->DoSettingAct();
    GridManager->InitCoinOccupied();
    CoinManager->CheckBattleReadyCoinAlive();

    TurnProgressing();
}