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

    if (IsValid(CoinManager))
    {
        CoinManager->OnAllCoinDead.BindUObject(this, &UBattleManagerWSubsystem::GameOver);
    }
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
    RandomStateArray.SetNum(BATTLE_COIN_MAX);
    for (FRandomState& RandomState : RandomStateArray)
    {
        RandomState.RandomFace = EFaceState::None;
        RandomState.RandomGrid = FGridPoint(-1, -1);
    }

    if (!IsValid(GridManager) || GridManager->GridXSize <= 0 || GridManager->GridYSize <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[Battle] 랜덤 상태 생성 실패: GridManager 또는 그리드 크기가 유효하지 않습니다."));
        return;
    }

    TArray<FGridPoint> CandidateCells;
    for (int32 GridY = 0; GridY < GridManager->GridYSize; ++GridY)
    {
        for (int32 GridX = 0; GridX < GridManager->GridXSize; ++GridX)
        {
            const FGridPoint Candidate(GridX, GridY);
            if (GridManager->CanCoinOccupyCell(Candidate))
            {
                CandidateCells.Add(Candidate);
            }
        }
    }

    // 중복 재추첨 while문 대신 후보를 한 번 섞어, 빈 셀이 부족해도 무한 루프가 생기지 않게 합니다.
    for (int32 Index = 0; Index < CandidateCells.Num(); ++Index)
    {
        const int32 SwapIndex = FMath::RandRange(Index, CandidateCells.Num() - 1);
        CandidateCells.Swap(Index, SwapIndex);
    }

    const int32 StateCount = FMath::Min(BATTLE_COIN_MAX, CandidateCells.Num());
    for (int32 Index = 0; Index < StateCount; ++Index)
    {
        RandomStateArray[Index].RandomFace = FMath::RandBool() ? EFaceState::Front : EFaceState::Back;
        RandomStateArray[Index].RandomGrid = CandidateCells[Index];
    }

    if (StateCount < BATTLE_COIN_MAX)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Battle] Coin 랜덤 배치 가능 셀이 부족합니다. 생성=%d 필요=%d"),
            StateCount, BATTLE_COIN_MAX);
    }
}

void UBattleManagerWSubsystem::MatchCoinsToRandomState()
{
    if (!IsValid(CoinManager) || !IsValid(GridManager))
    {
        return;
    }

    for (int32 ReadySlotIndex = 0; ReadySlotIndex < BATTLE_COIN_MAX; ++ReadySlotIndex)
    {
        ACoinActor* Coin = CoinManager->GetRuntimeCoinAtReadySlot(ReadySlotIndex);
        if (!IsValid(Coin))
        {
            continue;
        }

        if (!RandomStateArray.IsValidIndex(ReadySlotIndex))
        {
            UE_LOG(LogTemp, Error, TEXT("[Battle] ReadySlot=%d 랜덤 상태 인덱스가 없습니다."), ReadySlotIndex + 1);
            continue;
        }

        const FRandomState& RandomState = RandomStateArray[ReadySlotIndex];
        if (RandomState.RandomFace == EFaceState::None ||
            !GridManager->CanCoinOccupyCell(RandomState.RandomGrid))
        {
            UE_LOG(LogTemp, Error,
                TEXT("[Battle] ReadySlot=%d CoinID=%d 랜덤 상태 매칭 건너뜀: Face=%d Grid=(%d,%d)"),
                ReadySlotIndex + 1,
                Coin->GetCoinID(),
                static_cast<int32>(RandomState.RandomFace),
                RandomState.RandomGrid.GridX,
                RandomState.RandomGrid.GridY);
            continue;
        }

        if (!GridManager->TryOccupyCoinCell(RandomState.RandomGrid, Coin))
        {
            UE_LOG(LogTemp, Error, TEXT("[Battle] ReadySlot=%d CoinID=%d 그리드 점유에 실패했습니다."),
                ReadySlotIndex + 1, Coin->GetCoinID());
            continue;
        }

        Coin->SetCoinFace(RandomState.RandomFace);
        Coin->SetGridPoint(RandomState.RandomGrid);
    }
}

void UBattleManagerWSubsystem::DoCoinReadyPhase()
{
    LockLever(EBattleLeverLockReason::PhaseTransition);
	if (IsValid(CoinManager))
	{
		CoinManager->SetCoinReadyPhase(true);
	}
	if (IsValid(ItemManager))
	{
		ItemManager->SetPhase(false);
	}
    UnlockLeverAfter(LeverLockTime + 1.0f);
}

void UBattleManagerWSubsystem::DoCoinBehaviorPhase()
{
    if (!IsValid(CoinManager) || !IsValid(GridManager))
    {
        UE_LOG(LogTemp, Error, TEXT("[Battle] CoinBehaviorPhase 진입 실패: CoinManager 또는 GridManager가 유효하지 않습니다."));
        UnlockLever();
        return;
    }

    CoinManager->SetCoinReadyPhase(false);
    CoinManager->InstantiateReadyCoinActors();
    MatchCoinsToRandomState();
    
    // TODO: 카드 조건 검사가 Tick 기반으로 이관되면 호출 경로를 복구합니다.
    // if (StageCardManager)
    // {
    //     StageCardManager->ExecuteCardsEffect();
    // }

    if (IsValid(CoinActionManager))
    {
        // DB 코인을 배치하는 동안 입력을 잠그고, 실제 착지가 끝난 콜백에서 다시 엽니다.
        CoinActionManager->SetPhase(false);
    }

    if (IsValid(ItemManager))
    {
        // 진입 연출이 끝나기 전에는 아이템 입력을 받지 않습니다.
        ItemManager->SetPhase(false);
    }

    if (IsValid(ActingManager))
    {
        ActingManager->WaitTeleportUntilLeverDown(
            LeverLockTime,
            FSimpleDelegate::CreateUObject(this, &UBattleManagerWSubsystem::HandleCoinBehaviorEntryFinished)
        );
        return;
    }

    HandleCoinBehaviorEntryFinished();
}

void UBattleManagerWSubsystem::HandleCoinBehaviorEntryFinished()
{
    if (bIsStageEnded || CurrentPhase != EPhaseState::CoinBehaviorPhase)
    {
        return;
    }

    // 코인의 실제 착지가 모두 끝난 뒤부터만 호버·클릭 행동 입력을 허용합니다.
    if (IsValid(CoinActionManager))
    {
        CoinActionManager->SetPhase(true);
    }

    if (IsValid(ItemManager))
    {
        ItemManager->SetPhase(true);
        ItemManager->CoinBindsToItemMan();
    }

    if (ABossActor* Boss = BossManager ? BossManager->GetCurrentBoss() : nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Battle] DoCoinBehaviorPhase OnPlayerPhaseStart broadcast, GimmickCount=%d"), Boss->GetGimmickList().Num());
        for (UBossGimmickBase* G : Boss->GetGimmickList())
        {
            if (IsValid(G)) G->OnPlayerPhaseStart(Boss);
        }
    }

    UnlockLever();
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

    if (IsValid(CoinManager))
    {
        CoinManager->SetCoinReadyPhase(false);
        CoinManager->CheckBattleReadyCoinAlive();
    }

    if (IsValid(GridManager))
    {
        GridManager->InitGrids();
    }

    // 매 턴 SettingPhase에서 새 상태를 만들며, 다음 BehaviorPhase에서는 이전 점유 위치를 재사용하지 않습니다.
    GenerateRandomStates();

    if (IsValid(ActingManager))
    {
        ActingManager->DoSettingAct();
    }

    if (!IsValid(BossManager))
    {
        UE_LOG(LogTemp, Error, TEXT("[Battle] SettingPhase 진행 실패: BossManager가 유효하지 않습니다."));
        return;
    }

    BossManager->StartBossSetting();
    TSoftClassPtr<ABase_PatternVisualActor> VisualClass = BossManager->GetCurrentPatternVisualClass();

    if (!VisualClass.IsNull())
    {
        if (IsValid(ActingManager))
        {
            ActingManager->PrepareBossVisualActor(VisualClass);
        }
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
