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
#include "BossActor.h"
#include "CrossingLevelGISubsystem.h"
#include "UseableItemWSubsystem.h"
#include "GridManagerSubsystem.h"
#include "BattleLevelActingWSubsystem.h"
#include "SoundManagerWSubsystem.h"
#include "CoinActionManagementWSubsystem.h"
#include "DataManagerSubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Subsystem/StageCardWSubsystem.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "TemplateFunction_Utils.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "UI/W_BattlePhaseAndTurnDisplayUI.h"
#include "UI/W_StageEnd.h"

#define BATTLE_COIN_MAX 10

namespace
{
    constexpr int32 StageClearFlag = 0;
    constexpr int32 GameOverFlag = 1;
    constexpr int32 GameClearFlag = 2;
    constexpr int32 GameClearBattleLevelIndex = 4;
}

void UBattleManagerWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CoinManager = Collection.InitializeDependency<UCoinManagementWSubsystem>();
    GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();
    ItemManager = Collection.InitializeDependency<UUseableItemWSubsystem>();
    ActingManager = Collection.InitializeDependency<UBattleLevelActingWSubsystem>();
    CoinActionManager = Collection.InitializeDependency<UCoinActionManagementWSubsystem>();
    BossManager = Collection.InitializeDependency<UBossManagerSubsystem>();
    SoundManager = Collection.InitializeDependency<USoundManagerWSubsystem>();

    RandomStateArray.SetNum(BATTLE_COIN_MAX);

    TurnStackInit();
}

bool UBattleManagerWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer)) 
    {
        return false;
    }
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

    StageCardManager = InWorld.GetSubsystem<UStageCardWSubsystem>();

    ActingManager->OnCoinLanded.BindLambda([this]()
    {
        if (BossManager)
            BossManager->BroadcastCoinLanded();
        if (SoundManager)
            SoundManager->PlayCoinTeleportSound();
    });

    if (BossManager)
    {
        if (BossManager->SpawnPreparedBoss())
        {
            if (ABossActor* Boss = BossManager->GetCurrentBoss())
            {
                Boss->OnBossAttackEnded.AddDynamic(this, &UBattleManagerWSubsystem::DoSettingTurn);
                Boss->OnBossDeathStarted.AddDynamic(this, &UBattleManagerWSubsystem::BossDeathStarted);
                Boss->OnBossDead.AddDynamic(this, &UBattleManagerWSubsystem::StageEnded);
            }
        }
    }

    if(CoinManager)
    {
        CoinManager->OnAllCoinDead.BindUObject(this, &UBattleManagerWSubsystem::GameOver);
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

bool UBattleManagerWSubsystem::StartBattleFromLever(float BattleLeverEndTime) { 
   
    if (bIsStageEnded)
    {
        return false;
    }

    if (LeverGateState != EBattleLeverGateState::WaitingForLever)
    {
        return false;
    }

    if (CurrentTurn != ETurnState::CoinReadyTurn &&
        CurrentTurn != ETurnState::CoinSelectTurn)
    {
        return false;
    }

    LeverLockTime = BattleLeverEndTime;

    LockLever(EBattleLeverLockReason::TurnTransition);
    TurnProgressing();

    return true;
}

void UBattleManagerWSubsystem::LockLever(EBattleLeverLockReason Reason)
{
    LeverGateState = EBattleLeverGateState::Locked;
    LeverLockReason = Reason;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(LeverUnlockTimer);
    }
}

void UBattleManagerWSubsystem::UnlockLever()
{
    LeverGateState = EBattleLeverGateState::WaitingForLever;
    LeverLockReason = EBattleLeverLockReason::None;
}

void UBattleManagerWSubsystem::UnlockLeverAfter(float DelaySeconds)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(LeverUnlockTimer);

        World->GetTimerManager().SetTimer(
            LeverUnlockTimer,
            this,
            &UBattleManagerWSubsystem::UnlockLever,
            DelaySeconds,
            false
        );
    }
}

void UBattleManagerWSubsystem::TurnProgressing()
{
    if(bIsStageEnded) return;

    //세팅 턴 -> 코인 레디턴 -> 코인 설렉트턴 -> 보스 턴 
    TurnManageMentStack.Pop();
    CurrentTurn = TurnManageMentStack.Top();

    OnTurnChanged.Broadcast(CurrentTurn);

    if (CurrentTurn != ETurnState::SettingTurn)
    {
        if (!BattlePhaseAndTurnDisplayWidgetInstance)
        {
            const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
            UClass* BattlePhaseAndTurnDisplayWidgetClass = Settings ? Settings->BattlePhaseAndTurnDisplayWidgetClass.LoadSynchronous() : nullptr;
            if (BattlePhaseAndTurnDisplayWidgetClass)
            {
                BattlePhaseAndTurnDisplayWidgetInstance = CreateWidget<UW_BattlePhaseAndTurnDisplayUI>(GetWorld(), BattlePhaseAndTurnDisplayWidgetClass);
                if (BattlePhaseAndTurnDisplayWidgetInstance)
                {
                    BattlePhaseAndTurnDisplayWidgetInstance->AddToViewport(90);
                }
            }
        }

        if (BattlePhaseAndTurnDisplayWidgetInstance)
        {
            BattlePhaseAndTurnDisplayWidgetInstance->SetTurnDisplay(CurrentTurn, TurnCount);
        }
    }

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
            Coin->SetUIVisibility(true);

            GridManager->GetGridActor(RandomStateArray[StateIndex].RandomGrid)->SetOccupied(true, EGridOccupyingType::Coin, Coin);
            
            StateIndex++; // 코인을 찾았을 때만 다음 랜덤 상태로
        }
    }

    ActingManager->WaitTeleportUntilLeverDown();
}

void UBattleManagerWSubsystem::DoCoinReadyTurn()
{
    LockLever(EBattleLeverLockReason::TurnTransition);
    CoinManager->SetCoinReadyTurn(true);
    UnlockLeverAfter(LeverLockTime + 1.0f);
}

void UBattleManagerWSubsystem::DoCoinSelectTurn()
{
    CoinManager->SetCoinReadyTurn(false);
    
    MatchCoinsToRandomState();
    
    if (StageCardManager)
    {
        StageCardManager->ExecuteCardsEffect();
    }

    CoinActionManager->SetTurn(true);
    ItemManager->SetTurn(true);
    ItemManager->CoinBindsToItemMan();

    UnlockLeverAfter(LeverLockTime + 3.0f);

    if (ABossActor* Boss = BossManager ? BossManager->GetCurrentBoss() : nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Battle] DoCoinSelectTurn OnPlayerTurnStart broadcast, GimmickCount=%d"), Boss->GetGimmickList().Num());
        for (UBossGimmickBase* G : Boss->GetGimmickList())
        {
            if (IsValid(G)) G->OnPlayerTurnStart(Boss);
        }
    }

}

void UBattleManagerWSubsystem::DoBossTurn()
{
     if (ABossActor* Boss = BossManager ? BossManager->GetCurrentBoss() : nullptr)
    for (UBossGimmickBase* G : Boss->GetGimmickList())
    {
        if (IsValid(G)) G->OnPlayerTurnEnd(Boss);
    }

    CoinActionManager->SetTurn(false);
    ItemManager->SetTurn(false);
    ActingManager->PlayBossPatternAct();
    BossManager->ExecuteCurrentPattern();
    TurnStackInit();
}

void UBattleManagerWSubsystem::DoSettingTurn()
{
    if (bIsStageEnded) return;

    TurnCount++;

    if (StageCardManager)
    {
        StageCardManager->SettingDoSettingTurn();
    }

    GenerateRandomStates();
    ActingManager->DoSettingAct();
    GridManager->InitGrids();
    CoinManager->CheckBattleReadyCoinAlive();
    BossManager->StartBossSetting();
    TSoftClassPtr<ABase_PatternVisualActor> VisualClass = BossManager->GetCurrentPatternVisualClass();

    if (!VisualClass.IsNull())
    {
        ActingManager->PrepareBossVisualActor(VisualClass);
    }

    TurnProgressing();
}

void UBattleManagerWSubsystem::StageEnded()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    ULevelGISubsystem* LevelManager = GameInstance ? GameInstance->GetSubsystem<ULevelGISubsystem>() : nullptr;
    const int32 StageEndFlag = LevelManager && LevelManager->GetBattleLevelIndex() >= GameClearBattleLevelIndex
        ? GameClearFlag
        : StageClearFlag;

    if (!TryEndStage(StageEndFlag)) return;

    ShowStageEndWidget(StageEndFlag);
}

void UBattleManagerWSubsystem::BossDeathStarted()
{
    if (CoinActionManager)
    {
        CoinActionManager->StopActionSequenceForStageEnd();
    }

    if (ItemManager)
    {
        ItemManager->SetTurn(false);
    }

    if (ActingManager)
    {
        ActingManager->StopCoinActionAct();
        ActingManager->OnBossDeadAct.ExecuteIfBound();
    }
}

void UBattleManagerWSubsystem::GameOver()
{
    if (!TryEndStage(GameOverFlag)) return;

    ShowStageEndWidget(GameOverFlag);
}

bool UBattleManagerWSubsystem::TryEndStage(int32 StageEndFlag)
{
    if (bIsStageEnded)
    {
        return false;
    }

    bIsStageEnded = true;

    if (BattlePhaseAndTurnDisplayWidgetInstance)
    {
        BattlePhaseAndTurnDisplayWidgetInstance->RemoveFromParent();
        BattlePhaseAndTurnDisplayWidgetInstance = nullptr;
    }

    if (StageEndFlag == StageClearFlag)
    {
        AddStageClearRefundToMoney();
    }

    OnStageEnded.Broadcast(StageEndFlag);
    return true;
}

void UBattleManagerWSubsystem::AddStageClearRefundToMoney()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    UMoneyGISubsystem* MoneyManager = GameInstance ? GameInstance->GetSubsystem<UMoneyGISubsystem>() : nullptr;
    if (!MoneyManager)
    {
        return;
    }

    if (CoinManager)
    {
        MoneyManager->AddStageRefund(EMoneyRecordType::Coin, CoinManager->CalculateCoinPrice(), CoinManager->CalculateCoinCount());
    }

    if (ItemManager)
    {
        MoneyManager->AddStageRefund(EMoneyRecordType::Item, ItemManager->CalculateItemPrice(), ItemManager->CalculateItemCount());
    }

    /*
    if (StageCardManager)
    {
        MoneyManager->AddStageRefund(EMoneyRecordType::Card, StageCardManager->GetCardPrice(), StageCardManager->GetCardCount());
    }
    */

    ULevelGISubsystem* LevelManager = GameInstance->GetSubsystem<ULevelGISubsystem>();
    UDataManagerSubsystem* DataManager = GameInstance->GetSubsystem<UDataManagerSubsystem>();
    if (LevelManager && DataManager)
    {
        FStageRewardData Reward;
        if (DataManager->TryGetStageReward(LevelManager->GetBattleLevelIndex(), Reward))
        {
            MoneyManager->AddRewardMoney(Reward.RewardGold);
        }
    }
}

void UBattleManagerWSubsystem::ShowStageEndWidget(int32 StageEndFlag)
{
    if (StageEndWidgetInstance && StageEndWidgetInstance->IsInViewport())
    {
        return;
    }

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    UClass* StageEndWidgetClass = Settings ? Settings->StageEndWidgetClass.LoadSynchronous() : nullptr;
    if (!StageEndWidgetClass)
    {
        return;
    }

    StageEndWidgetInstance = CreateWidget<UW_StageEnd>(GetWorld(), StageEndWidgetClass);
    if (!StageEndWidgetInstance)
    {
        return;
    }

    StageEndWidgetInstance->SetStageEndFlag(StageEndFlag);
    StageEndWidgetInstance->AddToViewport(100);
}
