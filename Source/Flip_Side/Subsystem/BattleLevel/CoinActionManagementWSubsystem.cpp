#include "Subsystem/BattleLevel/CoinActionManagementWSubsystem.h"

#include "Actors/Boss/BossActor.h"
#include "Actors/CoinActor.h"
#include "Actors/Component_Status.h"
#include "Actors/GridActor.h"
#include "Actors/Others/Base_OtherActor.h"
#include "Engine/World.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Objects/Weapon_Action.h"
#include "Subsystem/BattleLevel/ActionLogicRegistryGISubsystem.h"
#include "Subsystem/BattleLevel/BattleLevelActingWSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "TimerManager.h"

void UCoinActionManagementWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();
	SelectedAction = NewObject<UWeapon_Action>(this);
	ResetActionState();
}

void UCoinActionManagementWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (IsValid(GridManager))
	{
		GridManager->OnGridClickedForCoin.BindDynamic(
			this, &UCoinActionManagementWSubsystem::ExecuteGridAction);
	}
}

bool UCoinActionManagementWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	const UWorld* World = Cast<UWorld>(Outer);
	return IsValid(World) && World->GetName().Contains(TEXT("L_Stage"));
}

void UCoinActionManagementWSubsystem::SetPhase(bool bIsPhase)
{
	bIsCorrectPhase = bIsPhase;
	if (!bIsPhase)
	{
		StopActionSequenceForStageEnd();
	}
}

void UCoinActionManagementWSubsystem::StopActionSequenceForStageEnd()
{
	bIsCorrectPhase = false;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CommonVFXTimerHandle);
	}
	if (UBattleLevelActingWSubsystem* ActingManager = GetActingManager())
	{
		ActingManager->StopCoinActionAct();
	}
	if (IsValid(SelectedAction) && IsValid(SelectedAction->GetCasterCoin()))
	{
		SelectedAction->GetCasterCoin()->SetCoinIsActing(false);
	}
	ResetActionState();
}

void UCoinActionManagementWSubsystem::CancelSelectWeapon()
{
	if (bActionSequenceActive)
	{
		return;
	}
	ResetActionState();
}

void UCoinActionManagementWSubsystem::ResetActionState(bool bResetSelectedAction)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CommonVFXTimerHandle);
	}

	ClearBossOutline();
	ClearValidAbilityTargets();
	bActionSequenceActive = false;
	bPendingFailedVFX = false;
	PipelineStage = ECoinWeaponPipelineStage::None;
	CurrentAbilityIndex = 0;
	PendingAbilityIndex = INDEX_NONE;
	PendingSelectionCount = 0;
	RemainingAttackCount = 0;
	CurrentInputState = EActionInputState::None;

	if (bResetSelectedAction && IsValid(SelectedAction))
	{
		SelectedAction->ResetAction();
	}
	if (IsValid(GridManager))
	{
		GridManager->ResetBattleCoinPreview();
		GridManager->SetGridClickFlag(EGridClickFlag::None);
	}
}

bool UCoinActionManagementWSubsystem::RebuildSelectedAction(ACoinActor* CasterCoin)
{
	if (!IsValid(SelectedAction) || !IsValid(CasterCoin) || !IsValid(CasterCoin->StatComponent))
	{
		return false;
	}

	const FFaceData* WeaponDefinition = CasterCoin->GetCurrentWeaponDefinition();
	if (!WeaponDefinition)
	{
		return false;
	}

	const FWeaponActionSnapshot Snapshot = CasterCoin->StatComponent->BuildActionSnapshot(
		CasterCoin->GetCoinDecidedFace());
	if (!SelectedAction->InitializeAction(CasterCoin, Snapshot, *WeaponDefinition))
	{
		return false;
	}
	return RefreshRangeTargets();
}

bool UCoinActionManagementWSubsystem::RefreshSnapshotIfStale()
{
	if (!IsValid(SelectedAction))
	{
		return false;
	}
	if (SelectedAction->IsSnapshotCurrent())
	{
		return true;
	}
	return RebuildSelectedAction(SelectedAction->GetCasterCoin());
}

bool UCoinActionManagementWSubsystem::RefreshRangeTargets()
{
	if (!IsValid(GridManager) || !IsValid(SelectedAction) ||
		!IsValid(SelectedAction->GetCasterCoin()))
	{
		return false;
	}

	const FGridPoint Origin = SelectedAction->GetCasterCoin()->GetDecidedGrid();
	TArray<FGridPoint> AttackCells;
	ABossActor* AttackBoss = nullptr;
	GridManager->CollectAttackRangeTargets(
		Origin, SelectedAction->GetSnapshot().AttackAreaSpec, AttackCells, AttackBoss);
	SelectedAction->SetAttackTargets(AttackCells, AttackBoss);

	TArray<FGridPoint> AbilityCells;
	if (SelectedAction->GetSnapshot().bHasAbilityArea)
	{
		FObjectOnGridInfo IgnoredObjects;
		GridManager->CollectAbilityRangeTargets(
			Origin, SelectedAction->GetSnapshot().AbilityAreaSpec, AbilityCells, IgnoredObjects);
	}
	SelectedAction->SetAbilityCells(AbilityCells);
	return true;
}

UActionLogicRegistryGISubsystem* UCoinActionManagementWSubsystem::GetLogicRegistry() const
{
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
	return IsValid(GameInstance)
		? GameInstance->GetSubsystem<UActionLogicRegistryGISubsystem>()
		: nullptr;
}

const FWeaponLogicSet* UCoinActionManagementWSubsystem::GetCurrentLogicSet() const
{
	UActionLogicRegistryGISubsystem* Registry = GetLogicRegistry();
	return IsValid(Registry) && IsValid(SelectedAction)
		? Registry->FindWeaponLogic(SelectedAction->GetSnapshot().WeaponID)
		: nullptr;
}

UBattleLevelActingWSubsystem* UCoinActionManagementWSubsystem::GetActingManager() const
{
	UWorld* World = GetWorld();
	return IsValid(World) ? World->GetSubsystem<UBattleLevelActingWSubsystem>() : nullptr;
}

void UCoinActionManagementWSubsystem::SetSelectedWeapon(ACoinActor* HoveredCoin)
{
	if (!bIsCorrectPhase || bActionSequenceActive || !IsValid(HoveredCoin) ||
		HoveredCoin->GetCoinIsActed() || !IsValid(HoveredCoin->StatComponent))
	{
		return;
	}

	ResetActionState();
	if (!RebuildSelectedAction(HoveredCoin))
	{
		return;
	}

	HoveredCoin->SetAttackRangeBracketVisible(true);
	if (ABossActor* Boss = SelectedAction->GetAttackBoss(); IsValid(Boss))
	{
		Boss->DisPlayOutline();
	}
}

void UCoinActionManagementWSubsystem::ExecuteSelectedWeapon(ACoinActor* ClickedCoin)
{
	if (!bIsCorrectPhase || !IsValid(ClickedCoin))
	{
		return;
	}

	if (CurrentInputState == EActionInputState::WaitingForCoinClick)
	{
		if (!ValidTargetCoins.Contains(ClickedCoin) || !IsValid(SelectedAction))
		{
			PlayFailedVFX();
			return;
		}

		TArray<ACoinActor*> Coins{ ClickedCoin };
		SelectedAction->SetCurrentAbilityTargets(Coins, TArray<ABase_OtherActor*>(), nullptr);
		SelectedAction->MarkAbilityActorSelected(ClickedCoin);
		CompleteManualAbilitySelection();
		return;
	}

	if (bActionSequenceActive || ClickedCoin->GetCoinIsActed())
	{
		PlayFailedVFX();
		return;
	}

	if (!IsValid(SelectedAction) || SelectedAction->GetCasterCoin() != ClickedCoin)
	{
		SetSelectedWeapon(ClickedCoin);
	}
	if (!IsValid(SelectedAction) || SelectedAction->GetCasterCoin() != ClickedCoin ||
		!RefreshSnapshotIfStale())
	{
		PlayFailedVFX();
		return;
	}

	const FWeaponLogicSet* LogicSet = GetCurrentLogicSet();
	if (!LogicSet || !LogicSet->AttackLogic)
	{
		// WeaponID 10 창의 적처럼 후구현 대상으로 남긴 무기는 행동을 소비하지 않습니다.
		PlayFailedVFX();
		return;
	}

	StartCoinActionSequence();
}

void UCoinActionManagementWSubsystem::HandleCoinUnHovered()
{
	if (!bIsCorrectPhase || bActionSequenceActive)
	{
		return;
	}
	ResetActionState();
}

void UCoinActionManagementWSubsystem::StartCoinActionSequence()
{
	ACoinActor* CasterCoin = IsValid(SelectedAction) ? SelectedAction->GetCasterCoin() : nullptr;
	if (!bIsCorrectPhase || bActionSequenceActive || !IsValid(CasterCoin))
	{
		return;
	}

	ClearBossOutline();
	CasterCoin->SetAttackRangeBracketVisible(false);
	CasterCoin->SetCoinIsActed(true);
	CasterCoin->SetCoinIsActing(true);
	bActionSequenceActive = true;
	CurrentInputState = EActionInputState::ExecutingAction;
	PipelineStage = ECoinWeaponPipelineStage::BeforeAttack;
	CurrentAbilityIndex = 0;

	if (UWorld* World = GetWorld())
	{
		if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
		{
			SoundManager->PlayCoinClickSound();
		}
	}

	if (UBattleLevelActingWSubsystem* ActingManager = GetActingManager())
	{
		ActingManager->RaiseCoinForAction(
			CasterCoin, FSimpleDelegate::CreateUObject(this, &UCoinActionManagementWSubsystem::BeginRaisedAction));
	}
	else
	{
		BeginRaisedAction();
	}
}

void UCoinActionManagementWSubsystem::BeginRaisedAction()
{
	if (bActionSequenceActive)
	{
		AdvancePipeline();
	}
}

void UCoinActionManagementWSubsystem::AdvancePipeline()
{
	if (!bActionSequenceActive || !IsValid(SelectedAction))
	{
		return;
	}

	switch (PipelineStage)
	{
	case ECoinWeaponPipelineStage::BeforeAttack:
		if (AdvanceAbilitiesForTiming(EAbilityTiming::BeforeAttack))
		{
			return;
		}
		PipelineStage = ECoinWeaponPipelineStage::Attack;
		RemainingAttackCount = ResolveRepeatCount(
			GetCurrentLogicSet() ? GetCurrentLogicSet()->AttackRepeatCountSource : ERepeatCountSource::One);
		BeginAttackStep();
		break;

	case ECoinWeaponPipelineStage::OnHit:
		if (AdvanceAbilitiesForTiming(EAbilityTiming::OnHit))
		{
			return;
		}
		PipelineStage = ECoinWeaponPipelineStage::AfterAttackAlways;
		CurrentAbilityIndex = 0;
		AdvancePipeline();
		break;

	case ECoinWeaponPipelineStage::AfterAttackAlways:
		if (AdvanceAbilitiesForTiming(EAbilityTiming::AfterAttackAlways))
		{
			return;
		}
		FinishCoinActionSequence();
		break;

	default:
		break;
	}
}

bool UCoinActionManagementWSubsystem::AdvanceAbilitiesForTiming(EAbilityTiming Timing)
{
	const FWeaponLogicSet* LogicSet = GetCurrentLogicSet();
	if (!LogicSet)
	{
		return false;
	}

	while (LogicSet->AbilityLogics.IsValidIndex(CurrentAbilityIndex))
	{
		const FRegisteredAbilityLogic& AbilityLogic = LogicSet->AbilityLogics[CurrentAbilityIndex];
		if (AbilityLogic.Timing != Timing)
		{
			++CurrentAbilityIndex;
			continue;
		}

		if (AbilityLogic.TargetRule.SelectionMode == EAbilitySelectionMode::None ||
			AbilityLogic.TargetRule.SelectionMode == EAbilitySelectionMode::AllInRange)
		{
			ExecuteAbilityImmediately(AbilityLogic);
			++CurrentAbilityIndex;
			continue;
		}

		if (BeginManualAbilitySelection(AbilityLogic))
		{
			return true;
		}
		++CurrentAbilityIndex;
	}
	return false;
}

void UCoinActionManagementWSubsystem::BeginAttackStep()
{
	if (!bActionSequenceActive || !IsValid(SelectedAction))
	{
		return;
	}
	if (RemainingAttackCount <= 0)
	{
		PipelineStage = SelectedAction->GetExecutionState().TotalDamageDealt > 0
			? ECoinWeaponPipelineStage::OnHit
			: ECoinWeaponPipelineStage::AfterAttackAlways;
		CurrentAbilityIndex = 0;
		AdvancePipeline();
		return;
	}

	ACoinActor* CasterCoin = SelectedAction->GetCasterCoin();
	if (!IsValid(CasterCoin) || !IsValid(CasterCoin->StatComponent) ||
		CasterCoin->StatComponent->GetOnIsOnCC())
	{
		bPendingFailedVFX = true;
		FinishCoinActionSequence();
		return;
	}

	CurrentInputState = EActionInputState::ExecutingAction;
	if (UBattleLevelActingWSubsystem* ActingManager = GetActingManager())
	{
		ActingManager->ShakeCoinForAction(
			CasterCoin, FSimpleDelegate::CreateUObject(this, &UCoinActionManagementWSubsystem::ResolveAttackStep));
	}
	else
	{
		ResolveAttackStep();
	}
}

void UCoinActionManagementWSubsystem::ResolveAttackStep()
{
	if (!bActionSequenceActive || !IsValid(SelectedAction))
	{
		return;
	}

	SelectedAction->GetExecutionState().CurrentRepeatIndex++;
	const FWeaponAttackResult AttackResult = SelectedAction->ExecuteAttack();
	PlayCoinSpecificVFX();
	--RemainingAttackCount;

	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	const float Delay = IsValid(Settings) ? Settings->CommonVFXDelayAfterCoinVFX : 0.0f;
	auto ContinuePipeline = [this, AttackResult]()
	{
		PlayCommonVFX(AttackResult);
		BeginAttackStep();
	};

	if (Delay > 0.0f && IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().SetTimer(
			CommonVFXTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, MoveTemp(ContinuePipeline)),
			Delay,
			false);
	}
	else
	{
		ContinuePipeline();
	}
}

void UCoinActionManagementWSubsystem::ExecuteAbilityImmediately(
	const FRegisteredAbilityLogic& AbilityLogic)
{
	if (!IsValid(SelectedAction))
	{
		return;
	}

	BuildValidAbilityTargets(AbilityLogic.TargetRule);
	TArray<ACoinActor*> Coins;
	TArray<ABase_OtherActor*> Others;
	for (ACoinActor* Coin : ValidTargetCoins)
	{
		if (IsValid(Coin))
		{
			Coins.Add(Coin);
			SelectedAction->MarkAbilityActorSelected(Coin);
		}
	}
	for (ABase_OtherActor* Other : ValidTargetOthers)
	{
		if (IsValid(Other))
		{
			Others.Add(Other);
			SelectedAction->MarkAbilityActorSelected(Other);
		}
	}

	SelectedAction->SetCurrentAbilityTargets(Coins, Others, nullptr);
	SelectedAction->ExecuteAbility(AbilityLogic);
	ClearValidAbilityTargets();
}

bool UCoinActionManagementWSubsystem::BeginManualAbilitySelection(
	const FRegisteredAbilityLogic& AbilityLogic)
{
	PendingSelectionCount = AbilityLogic.TargetRule.SelectionMode == EAbilitySelectionMode::Single
		? 1
		: ResolveRepeatCount(AbilityLogic.TargetRule.RepeatCountSource);
	if (PendingSelectionCount <= 0)
	{
		return false;
	}

	BuildValidAbilityTargets(AbilityLogic.TargetRule);
	if (ValidTargetCoins.IsEmpty() && ValidTargetGrids.IsEmpty() && ValidTargetOthers.IsEmpty())
	{
		ClearValidAbilityTargets();
		return false;
	}

	PendingAbilityIndex = CurrentAbilityIndex;
	ConfigureInputForRule(AbilityLogic.TargetRule);
	return CurrentInputState == EActionInputState::WaitingForCoinClick ||
		CurrentInputState == EActionInputState::WaitingForGridClick ||
		CurrentInputState == EActionInputState::WaitingForOtherClick;
}

void UCoinActionManagementWSubsystem::CompleteManualAbilitySelection()
{
	const FWeaponLogicSet* LogicSet = GetCurrentLogicSet();
	if (!LogicSet || !LogicSet->AbilityLogics.IsValidIndex(PendingAbilityIndex) ||
		!IsValid(SelectedAction))
	{
		FinishCoinActionSequence();
		return;
	}

	const FRegisteredAbilityLogic& AbilityLogic = LogicSet->AbilityLogics[PendingAbilityIndex];
	SelectedAction->GetExecutionState().CurrentRepeatIndex++;
	SelectedAction->ExecuteAbility(AbilityLogic);
	--PendingSelectionCount;
	ClearValidAbilityTargets();

	if (PendingSelectionCount > 0)
	{
		BuildValidAbilityTargets(AbilityLogic.TargetRule);
		if (!ValidTargetCoins.IsEmpty() || !ValidTargetGrids.IsEmpty() || !ValidTargetOthers.IsEmpty())
		{
			ConfigureInputForRule(AbilityLogic.TargetRule);
			return;
		}
	}

	CurrentAbilityIndex = PendingAbilityIndex + 1;
	PendingAbilityIndex = INDEX_NONE;
	PendingSelectionCount = 0;
	CurrentInputState = EActionInputState::ExecutingAction;
	if (IsValid(GridManager))
	{
		GridManager->SetGridClickFlag(EGridClickFlag::None);
	}
	AdvancePipeline();
}

void UCoinActionManagementWSubsystem::BuildValidAbilityTargets(const FAbilityTargetRule& Rule)
{
	ClearValidAbilityTargets();
	if (!IsValid(GridManager) || !IsValid(SelectedAction) ||
		!IsValid(SelectedAction->GetCasterCoin()))
	{
		return;
	}

	TArray<FGridPoint> AbilityCells;
	FObjectOnGridInfo Objects;
	if (SelectedAction->GetSnapshot().bHasAbilityArea)
	{
		GridManager->CollectAbilityRangeTargets(
			SelectedAction->GetCasterCoin()->GetDecidedGrid(),
			SelectedAction->GetSnapshot().AbilityAreaSpec,
			AbilityCells,
			Objects);
	}
	SelectedAction->SetAbilityCells(AbilityCells);

	if (Rule.HasTarget(EAbilityTargetFlags::Coin))
	{
		for (AActor* Actor : Objects.Coins)
		{
			ACoinActor* Coin = Cast<ACoinActor>(Actor);
			if (IsValidCoinTarget(Coin, Rule))
			{
				ValidTargetCoins.AddUnique(Coin);
			}
		}
	}

	for (AActor* Actor : Objects.Others)
	{
		ABase_OtherActor* Other = Cast<ABase_OtherActor>(Actor);
		if (IsValidOtherTarget(Other, Rule))
		{
			ValidTargetOthers.AddUnique(Other);
		}
	}

	if (Rule.HasTarget(EAbilityTargetFlags::EmptyGrid))
	{
		for (const FGridPoint& Cell : AbilityCells)
		{
			if (Rule.bCoinFieldOnly && GridManager->IsBossAreaCell(Cell))
			{
				continue;
			}
			AGridActor* Grid = GridManager->GetGridActor(Cell);
			if (IsValid(Grid) && !Grid->GetIsOccupied() &&
				!SelectedAction->WasAbilityActorSelected(Grid))
			{
				ValidTargetGrids.AddUnique(Grid);
			}
		}
	}
}

void UCoinActionManagementWSubsystem::ClearValidAbilityTargets()
{
	ValidTargetCoins.Reset();
	ValidTargetGrids.Reset();
	ValidTargetOthers.Reset();
}

bool UCoinActionManagementWSubsystem::IsValidCoinTarget(
	const ACoinActor* Coin,
	const FAbilityTargetRule& Rule) const
{
	if (!IsValid(Coin) || !IsValid(SelectedAction) || Coin->StatComponent == nullptr ||
		Coin->StatComponent->IsDead() || SelectedAction->WasAbilityActorSelected(Coin))
	{
		return false;
	}
	if (Rule.bExcludeCaster && Coin == SelectedAction->GetCasterCoin())
	{
		return false;
	}
	return !Rule.bCoinFieldOnly || !IsValid(GridManager) ||
		!GridManager->IsBossAreaCell(Coin->GetDecidedGrid());
}

bool UCoinActionManagementWSubsystem::IsValidOtherTarget(
	const ABase_OtherActor* Other,
	const FAbilityTargetRule& Rule) const
{
	if (!IsValid(Other) || !IsValid(SelectedAction) ||
		SelectedAction->WasAbilityActorSelected(Other))
	{
		return false;
	}
	if (Rule.bCoinFieldOnly && IsValid(GridManager) && IsValid(Other->GetOccupiedGrid()) &&
		GridManager->IsBossAreaCell(Other->GetOccupiedGrid()->GetGridPoint()))
	{
		return false;
	}

	return (Rule.HasTarget(EAbilityTargetFlags::Obstacle) && Other->GetOtherType() == EOthersType::Wall) ||
		(Rule.HasTarget(EAbilityTargetFlags::Installation) && Other->GetOtherType() == EOthersType::Turret);
}

int32 UCoinActionManagementWSubsystem::ResolveRepeatCount(ERepeatCountSource Source) const
{
	if (!IsValid(SelectedAction))
	{
		return 0;
	}
	switch (Source)
	{
	case ERepeatCountSource::WeaponPoint:
		return FMath::Max(0, SelectedAction->GetFinalBehaviorPoint());
	case ERepeatCountSource::WeaponCount:
		return FMath::Max(0, SelectedAction->GetFinalWeaponCount());
	case ERepeatCountSource::One:
	default:
		return 1;
	}
}

void UCoinActionManagementWSubsystem::ConfigureInputForRule(const FAbilityTargetRule& Rule)
{
	CurrentInputState = EActionInputState::ExecutingAction;
	if (Rule.HasTarget(EAbilityTargetFlags::Coin))
	{
		CurrentInputState = EActionInputState::WaitingForCoinClick;
		GridManager->SetGridClickFlag(EGridClickFlag::CoinAction);
	}
	else if (Rule.HasTarget(EAbilityTargetFlags::EmptyGrid))
	{
		CurrentInputState = EActionInputState::WaitingForGridClick;
		GridManager->SetGridClickFlag(EGridClickFlag::CoinAction);
	}
	else if (Rule.HasTarget(EAbilityTargetFlags::Obstacle) ||
		Rule.HasTarget(EAbilityTargetFlags::Installation))
	{
		CurrentInputState = EActionInputState::WaitingForOtherClick;
		GridManager->SetGridClickFlag(EGridClickFlag::None);
	}
}

bool UCoinActionManagementWSubsystem::TryExecuteOtherAction(ABase_OtherActor* TargetOther)
{
	if (CurrentInputState != EActionInputState::WaitingForOtherClick)
	{
		return false;
	}
	if (!ValidTargetOthers.Contains(TargetOther) || !IsValid(SelectedAction))
	{
		PlayFailedVFX();
		return true;
	}

	TArray<ABase_OtherActor*> Others{ TargetOther };
	SelectedAction->SetCurrentAbilityTargets(TArray<ACoinActor*>(), Others, nullptr);
	SelectedAction->MarkAbilityActorSelected(TargetOther);
	CompleteManualAbilitySelection();
	return true;
}

void UCoinActionManagementWSubsystem::ExecuteGridAction(AGridActor* TargetGrid)
{
	if (CurrentInputState != EActionInputState::WaitingForGridClick ||
		!ValidTargetGrids.Contains(TargetGrid) || !IsValid(SelectedAction))
	{
		if (CurrentInputState == EActionInputState::WaitingForGridClick)
		{
			PlayFailedVFX();
		}
		return;
	}

	SelectedAction->SetCurrentAbilityTargets(
		TArray<ACoinActor*>(), TArray<ABase_OtherActor*>(), TargetGrid);
	SelectedAction->MarkAbilityActorSelected(TargetGrid);
	CompleteManualAbilitySelection();
}

void UCoinActionManagementWSubsystem::TryCancelCurrentAction()
{
	if (!bActionSequenceActive ||
		(CurrentInputState != EActionInputState::WaitingForCoinClick &&
		 CurrentInputState != EActionInputState::WaitingForGridClick &&
		 CurrentInputState != EActionInputState::WaitingForOtherClick))
	{
		return;
	}
	FinishCoinActionSequence();
}

void UCoinActionManagementWSubsystem::CancelSingleCellAction(ACoinActor* ClickedCoin)
{
	if (!bActionSequenceActive || !IsValid(SelectedAction) ||
		ClickedCoin != SelectedAction->GetCasterCoin())
	{
		return;
	}
	TryCancelCurrentAction();
}

void UCoinActionManagementWSubsystem::FinishCoinActionSequence()
{
	if (!bActionSequenceActive || PipelineStage == ECoinWeaponPipelineStage::Finishing)
	{
		return;
	}
	PipelineStage = ECoinWeaponPipelineStage::Finishing;
	CurrentInputState = EActionInputState::ExecutingAction;
	ClearValidAbilityTargets();
	if (IsValid(GridManager))
	{
		GridManager->SetGridClickFlag(EGridClickFlag::None);
	}

	ACoinActor* CasterCoin = IsValid(SelectedAction) ? SelectedAction->GetCasterCoin() : nullptr;
	if (UBattleLevelActingWSubsystem* ActingManager = GetActingManager())
	{
		ActingManager->LowerCoinAfterAction(
			CasterCoin,
			FSimpleDelegate::CreateUObject(this, &UCoinActionManagementWSubsystem::HandleCoinActionLowerFinished));
	}
	else
	{
		HandleCoinActionLowerFinished();
	}
}

void UCoinActionManagementWSubsystem::HandleCoinActionLowerFinished()
{
	if (IsValid(SelectedAction) && IsValid(SelectedAction->GetCasterCoin()))
	{
		SelectedAction->GetCasterCoin()->SetCoinIsActing(false);
	}
	if (bPendingFailedVFX)
	{
		PlayFailedVFX();
	}
	ResetActionState();
}

void UCoinActionManagementWSubsystem::PlayCoinSpecificVFX()
{
	if (!IsValid(SelectedAction))
	{
		return;
	}

	const FFaceData& WeaponData = SelectedAction->GetWeaponData();
	if (IsValid(WeaponData.WeaponSFX))
	{
		if (UWorld* World = GetWorld())
		{
			if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
			{
				SoundManager->PlayCoinActionSound(WeaponData.WeaponSFX);
			}
		}
	}
	if (!IsValid(WeaponData.WeaponVFX))
	{
		return;
	}

	switch (WeaponData.WeaponVFXTarget)
	{
	case EWeaponVFXTarget::Caster:
		if (ACoinActor* Coin = SelectedAction->GetCasterCoin(); IsValid(Coin))
		{
			SpawnVFXAtLocation(WeaponData.WeaponVFX, Coin->GetActorLocation());
		}
		break;
	case EWeaponVFXTarget::TargetGrid:
		if (AGridActor* Grid = SelectedAction->GetTargetGrid(); IsValid(Grid))
		{
			const FVector2D XY = Grid->GetGridWorldXY();
			SpawnVFXAtLocation(WeaponData.WeaponVFX, FVector(XY.X, XY.Y, -80.0f));
		}
		break;
	case EWeaponVFXTarget::TargetCoin:
	case EWeaponVFXTarget::AffectedCoins:
		for (ACoinActor* Coin : SelectedAction->GetInRangeCoins())
		{
			if (IsValid(Coin))
			{
				SpawnVFXAtLocation(WeaponData.WeaponVFX, Coin->GetActorLocation());
				if (WeaponData.WeaponVFXTarget == EWeaponVFXTarget::TargetCoin)
				{
					break;
				}
			}
		}
		break;
	case EWeaponVFXTarget::TargetOther:
		if (ABase_OtherActor* Other = SelectedAction->GetTargetOther(); IsValid(Other))
		{
			SpawnVFXAtLocation(WeaponData.WeaponVFX, Other->GetActorLocation());
		}
		break;
	case EWeaponVFXTarget::Boss:
		if (ABossActor* Boss = SelectedAction->GetAttackBoss(); IsValid(Boss))
		{
			SpawnVFXAtLocation(WeaponData.WeaponVFX, Boss->GetActorLocation());
		}
		break;
	case EWeaponVFXTarget::RangeCells:
		if (IsValid(GridManager))
		{
			for (const FGridPoint& Cell : SelectedAction->GetAttackCells())
			{
				if (AGridActor* Grid = GridManager->GetGridActor(Cell); IsValid(Grid))
				{
					const FVector2D XY = Grid->GetGridWorldXY();
					SpawnVFXAtLocation(WeaponData.WeaponVFX, FVector(XY.X, XY.Y, -80.0f));
				}
			}
		}
		break;
	default:
		break;
	}
}

void UCoinActionManagementWSubsystem::PlayCommonVFX(const FWeaponAttackResult& AttackResult)
{
	if (!AttackResult.DidHit() || !IsValid(AttackResult.Boss))
	{
		return;
	}
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (IsValid(Settings))
	{
		SpawnVFXAtLocation(Settings->Boss_Hit_VFX.LoadSynchronous(), AttackResult.Boss->GetActorLocation());
	}
}

void UCoinActionManagementWSubsystem::PlayFailedVFX()
{
	ACoinActor* CasterCoin = IsValid(SelectedAction) ? SelectedAction->GetCasterCoin() : nullptr;
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!IsValid(CasterCoin) || !IsValid(Settings))
	{
		return;
	}
	SpawnVFXAtLocation(Settings->Coin_Logic_Failed_VFX.LoadSynchronous(), CasterCoin->GetActorLocation());
	if (UWorld* World = GetWorld())
	{
		if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
		{
			SoundManager->PlayCoinActionFailedSound();
		}
	}
}

void UCoinActionManagementWSubsystem::SpawnVFXAtLocation(
	UNiagaraSystem* VFX,
	const FVector& Location) const
{
	if (IsValid(VFX) && IsValid(GetWorld()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), VFX, Location);
	}
}

void UCoinActionManagementWSubsystem::ClearBossOutline()
{
	if (IsValid(SelectedAction))
	{
		if (ABossActor* Boss = SelectedAction->GetAttackBoss(); IsValid(Boss))
		{
			Boss->UnDisPlayOutline();
		}
		if (ACoinActor* Caster = SelectedAction->GetCasterCoin(); IsValid(Caster))
		{
			Caster->SetAttackRangeBracketVisible(false);
		}
	}
}
