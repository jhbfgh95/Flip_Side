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
#include "UI/W_StageEnd.h"

#define BATTLE_COIN_MAX 10

namespace
{
    constexpr int32 StageClearFlag = 0;
    constexpr int32 GameOverFlag = 1;
    constexpr int32 GameClearFlag = 2;
    constexpr int32 GameClearBattleLevelIndex = 2;
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

    PhaseStackInit();
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
                Boss->OnBossAttackEnded.AddDynamic(this, &UBattleManagerWSubsystem::HandleBossPhaseCompleted);
                Boss->OnBossDeathStarted.AddDynamic(this, &UBattleManagerWSubsystem::BossDeathStarted);
                Boss->OnBossDead.AddDynamic(this, &UBattleManagerWSubsystem::StageEnded);
            }
        }
    }

    // TODO: CoinBehaviorPhase에서 실 CoinActor 생성 흐름을 복구할 때 다시 바인딩합니다.
    // if(CoinManager)
    // {
    //     CoinManager->OnAllCoinDead.BindUObject(this, &UBattleManagerWSubsystem::GameOver);
    // }
    DoSettingPhase();
}

void UBattleManagerWSubsystem::PhaseStackInit()
{
    PhaseManagementStack.Push(EPhaseState::BossPhase);
    PhaseManagementStack.Push(EPhaseState::CoinBehaviorPhase);
    PhaseManagementStack.Push(EPhaseState::CoinReadyPhase);
    PhaseManagementStack.Push(EPhaseState::SettingPhase);

    CurrentPhase = PhaseManagementStack.Top();
}

EPhaseState UBattleManagerWSubsystem::GetCurrentPhase() const
{
    return PhaseManagementStack.Top();
}

bool UBattleManagerWSubsystem::RequestPhaseProgress(float TransitionLockTime)
{
    if (bIsStageEnded)
    {
        return false;
    }

    if (LeverGateState != EBattleLeverGateState::WaitingForLever)
    {
        return false;
    }

    if (CurrentPhase != EPhaseState::CoinReadyPhase &&
        CurrentPhase != EPhaseState::CoinBehaviorPhase)
    {
        return false;
    }

    LeverLockTime = FMath::Max(0.f, TransitionLockTime);

    LockLever(EBattleLeverLockReason::PhaseTransition);
    PhaseProgressing();

    return true;
}

bool UBattleManagerWSubsystem::StartBattleFromLever(float BattleLeverEndTime)
{
    const bool bPhaseProgressed = RequestPhaseProgress(BattleLeverEndTime);
    if (!bPhaseProgressed)
    {
        return false;
    }

    // TODO: 튜토리얼을 재구성할 때 새 UI 진행 버튼에 맞는 이벤트로 다시 연결합니다.
    // OnBattleTutorialLeverTriggered.Broadcast();
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

void UBattleManagerWSubsystem::PhaseProgressing()
{
    if(bIsStageEnded) return;

    // 세팅 페이즈 -> 코인 레디 페이즈 -> 코인 비헤이비어 페이즈 -> 보스 페이즈
    PhaseManagementStack.Pop();
    CurrentPhase = PhaseManagementStack.Top();

    OnPhaseChanged.Broadcast(CurrentPhase);

    switch(CurrentPhase)
    {
    case EPhaseState::SettingPhase:
        DoSettingPhase();
        break;
    case EPhaseState::CoinReadyPhase:
        DoCoinReadyPhase();
        break;
    case EPhaseState::CoinBehaviorPhase:
        DoCoinBehaviorPhase();
        break;
    case EPhaseState::BossPhase:
        DoBossPhase();
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
    // TODO: CoinBehaviorPhase에서 ReadyCoinData를 CoinActor로 인스턴싱한 뒤 복구합니다.
}

void UBattleManagerWSubsystem::DoCoinReadyPhase()
{
    LockLever(EBattleLeverLockReason::PhaseTransition);
	if (ItemManager)
	{
		ItemManager->SetPhase(false);
	}
    // TODO: CoinReadyPhase 리팩터링 완료 후 UI 입력 활성화를 여기서 제어합니다.
    // CoinManager->SetCoinReadyPhase(true);
    UnlockLeverAfter(LeverLockTime + 1.0f);
}

void UBattleManagerWSubsystem::DoCoinBehaviorPhase()
{
    // TODO: CoinBehaviorPhase 구현 전까지 Actor 기반 CoinReady 처리와 전송은 비활성화합니다.
    // CoinManager->SetCoinReadyPhase(false);
    // MatchCoinsToRandomState();
    
    // TODO: 카드 조건 검사가 Tick 기반으로 이관되면 호출 경로를 복구합니다.
    // if (StageCardManager)
    // {
    //     StageCardManager->ExecuteCardsEffect();
    // }

    if (IsValid(CoinActionManager))
    {
        CoinActionManager->SetPhase(true);
    }

    if (IsValid(ItemManager))
    {
        ItemManager->SetPhase(true);
        ItemManager->CoinBindsToItemMan();
    }

    UnlockLeverAfter(LeverLockTime + 3.0f);

    if (ABossActor* Boss = BossManager ? BossManager->GetCurrentBoss() : nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Battle] DoCoinBehaviorPhase OnPlayerPhaseStart broadcast, GimmickCount=%d"), Boss->GetGimmickList().Num());
        for (UBossGimmickBase* G : Boss->GetGimmickList())
        {
            if (IsValid(G)) G->OnPlayerPhaseStart(Boss);
        }
    }

}

void UBattleManagerWSubsystem::DoBossPhase()
{
     if (ABossActor* Boss = BossManager ? BossManager->GetCurrentBoss() : nullptr)
    for (UBossGimmickBase* G : Boss->GetGimmickList())
    {
        if (IsValid(G)) G->OnPlayerPhaseEnd(Boss);
    }

    if (IsValid(CoinActionManager))
    {
        CoinActionManager->SetPhase(false);
    }

    if (IsValid(ItemManager))
    {
        ItemManager->SetPhase(false);
    }

    if (IsValid(ActingManager))
    {
        ActingManager->PlayBossPatternAct();
    }

    if (IsValid(BossManager))
    {
        BossManager->ExecuteCurrentPattern();
    }
}

void UBattleManagerWSubsystem::DoSettingPhase()
{
    if (bIsStageEnded) return;

    TurnCount++;

    if (StageCardManager)
    {
        StageCardManager->SettingDoSettingPhase();
    }

    GenerateRandomStates();
    ActingManager->DoSettingAct();
    GridManager->InitGrids();
    // TODO: CoinBehaviorPhase에서 생존 CoinActor를 ReadyCoinData로 복귀시키는 로직을 추가합니다.
    // CoinManager->CheckBattleReadyCoinAlive();
    BossManager->StartBossSetting();
    TSoftClassPtr<ABase_PatternVisualActor> VisualClass = BossManager->GetCurrentPatternVisualClass();

    if (!VisualClass.IsNull())
    {
        ActingManager->PrepareBossVisualActor(VisualClass);
    }

    PhaseProgressing();
}

void UBattleManagerWSubsystem::HandleBossPhaseCompleted()
{
	if (bIsStageEnded)
	{
		return;
	}

	OnBossPhaseCompleted.Broadcast();
	PhaseStackInit();
	DoSettingPhase();
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
        ItemManager->SetPhase(false);
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
