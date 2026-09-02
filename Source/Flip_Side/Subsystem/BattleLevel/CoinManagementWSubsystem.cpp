#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"

#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"
#include "Subsystem/BattleLevel/CoinActionManagementWSubsystem.h"
#include "Actors/CoinActor.h"
#include "Actors/Component_Status.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Subsystems/Subsystem.h"
#include "Engine/World.h"

namespace
{
	constexpr int32 BloodPoweredBloodCannonWeaponID = 4;
	constexpr int32 RapidFreezerWeaponID = 7;
	constexpr int32 ShellGameGauntletWeaponID = 11;
	constexpr int32 AdrenalineInjectionPistolWeaponID = 14;
	constexpr int32 EmergencyResuscitationDeviceWeaponID = 16;
	constexpr int32 CrushingDrillWeaponID = 17;

	struct FTestCoinWeaponPair
	{
		int32 FrontWeaponID;
		int32 BackWeaponID;
	};

	// 기존 테스트 코인 3종을 유지하고, 신규 테스트 무기 3종은 별도 코인 슬롯으로 뒤에 추가합니다.
	constexpr FTestCoinWeaponPair TestCoinWeaponPairs[] =
	{
		{ BloodPoweredBloodCannonWeaponID, ShellGameGauntletWeaponID },
		{ ShellGameGauntletWeaponID, AdrenalineInjectionPistolWeaponID },
		{ AdrenalineInjectionPistolWeaponID, BloodPoweredBloodCannonWeaponID },
		{ EmergencyResuscitationDeviceWeaponID, CrushingDrillWeaponID },
		{ CrushingDrillWeaponID, RapidFreezerWeaponID },
		{ RapidFreezerWeaponID, EmergencyResuscitationDeviceWeaponID }
	};

	FAttackAreaSpec MakeForwardAttackAreaSpec(int32 Depth)
	{
		FAttackAreaSpec Spec;
		Spec.Pattern = EAttackAreaPattern::RectFromCell;
		Spec.AnchorCell = FGridPoint{ 0, 0 };
		Spec.AnchorMode = EAreaAnchor::UseAnchorCell;
		Spec.ParamA = 1;
		Spec.ParamB = FMath::Max(1, Depth);
		Spec.Side = EAreaSide::Up;
		Spec.Flags = 0;
		return Spec;
	}

	FAttackAreaSpec MakeSquareAbilityAreaSpec(int32 Radius)
	{
		FAttackAreaSpec Spec;
		Spec.Pattern = EAttackAreaPattern::CircleOnCell;
		Spec.AnchorCell = FGridPoint{ 0, 0 };
		Spec.AnchorMode = EAreaAnchor::UseAnchorCell;
		Spec.ParamA = FMath::Max(0, Radius);
		Spec.ParamB = 0;
		// 대상 필터링은 실제 능력 로직 연결 시 결정하고, 여기서는 순수 사거리만 보유합니다.
		Spec.Flags = 0;
		return Spec;
	}

	FWeaponFaceStats MakeTestWeaponFaceStats(int32 WeaponID)
	{
		FWeaponFaceStats FaceStats;
		FaceStats.WeaponID = WeaponID;

		// 무기 기획서 기준 수치와 사거리입니다. 실제 표시·행동 연결은 후속 작업에서 진행합니다.
		switch (WeaponID)
		{
		case BloodPoweredBloodCannonWeaponID:
			FaceStats.BaseNumericStats = { 2, 3, 0 };
			FaceStats.AttackAreaSpec = MakeForwardAttackAreaSpec(3);
			FaceStats.AbilityAreaSpec = MakeSquareAbilityAreaSpec(1);
			FaceStats.bHasAbilityArea = true;
			break;
		case RapidFreezerWeaponID:
			FaceStats.BaseNumericStats = { 1, 3, 0 };
			FaceStats.AttackAreaSpec = MakeForwardAttackAreaSpec(4);
			break;
		case ShellGameGauntletWeaponID:
			FaceStats.BaseNumericStats = { 3, 3, 0 };
			FaceStats.AttackAreaSpec = MakeForwardAttackAreaSpec(2);
			// 9x9 어느 위치에서든 전체 보드를 포함하는 전 범위 Spec입니다.
			FaceStats.AbilityAreaSpec = MakeSquareAbilityAreaSpec(8);
			FaceStats.bHasAbilityArea = true;
			break;
		case AdrenalineInjectionPistolWeaponID:
			FaceStats.BaseNumericStats = { 1, 1, 2 };
			FaceStats.AttackAreaSpec = MakeForwardAttackAreaSpec(2);
			FaceStats.AbilityAreaSpec = MakeSquareAbilityAreaSpec(1);
			FaceStats.bHasAbilityArea = true;
			break;
		case EmergencyResuscitationDeviceWeaponID:
			FaceStats.BaseNumericStats = { 1, 1, 0 };
			FaceStats.AttackAreaSpec = MakeForwardAttackAreaSpec(2);
			FaceStats.AbilityAreaSpec = MakeSquareAbilityAreaSpec(1);
			FaceStats.bHasAbilityArea = true;
			break;
		case CrushingDrillWeaponID:
			FaceStats.BaseNumericStats = { 1, 1, 2 };
			FaceStats.AttackAreaSpec = MakeForwardAttackAreaSpec(2);
			FaceStats.AbilityAreaSpec = MakeSquareAbilityAreaSpec(1);
			FaceStats.bHasAbilityArea = true;
			break;
		default:
			return FWeaponFaceStats();
		}

		return FaceStats;
	}

	FWeaponStatDisplayData MakeTestWeaponStatData(int32 WeaponID)
	{
		const FWeaponFaceStats FaceStats = MakeTestWeaponFaceStats(WeaponID);
		const FWeaponNumericStats& NumericStats = FaceStats.BaseNumericStats;
		return { NumericStats.AttackPoint, NumericStats.WeaponPoint, NumericStats.WeaponCnt };
	}
}

void UCoinManagementWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();
	CoinActionManager = Collection.InitializeDependency<UCoinActionManagementWSubsystem>();
	ReadyCoins.Init(FReadyCoinData(), MaxReadyCoinCount);
	RuntimeCoinsByReadySlot.SetNum(MaxReadyCoinCount);
}

void UCoinManagementWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (!InWorld.IsGameWorld())
	{
		return;
	}

	UGameInstance* GameInstance = InWorld.GetGameInstance();
	UCrossingLevelGISubsystem* CrossingLevelSubsystem = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UCrossingLevelGISubsystem>()
		: nullptr;

	TArray<FCoinTypeStructure> InventoryCoinSlots;
	if (IsValid(CrossingLevelSubsystem) && !CrossingLevelSubsystem->GetIsCoinEmpty())
	{
		const int32 MadeCoinCount = CrossingLevelSubsystem->GetMakedCoinNum();
		for (int32 SlotIndex = 0; SlotIndex < MadeCoinCount; ++SlotIndex)
		{
			InventoryCoinSlots.Add(CrossingLevelSubsystem->GetSlotCoin(SlotIndex));
		}
	}

	if (InventoryCoinSlots.IsEmpty())
	{
		CreateTestCoinSlots();
		return;
	}

	InitializeCoinSlots(InventoryCoinSlots);
}

bool UCoinManagementWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	const UWorld* World = Cast<UWorld>(Outer);
	return IsValid(World) && World->GetName().Contains(TEXT("L_Stage"));
}

void UCoinManagementWSubsystem::InitializeCoinSlots(const TArray<FCoinTypeStructure>& InCoinSlots)
{
	bUsingTestCoinData = false;
	CoinSlots.Reset();
	ReadyCoins.Init(FReadyCoinData(), MaxReadyCoinCount);

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
	const UDataManagerSubsystem* DataManager = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UDataManagerSubsystem>()
		: nullptr;

	for (int32 SlotIndex = 0; SlotIndex < InCoinSlots.Num(); ++SlotIndex)
	{
		const FCoinTypeStructure& SourceSlot = InCoinSlots[SlotIndex];
		if (SourceSlot.SameTypeCoinNum <= 0)
		{
			continue;
		}

		FBattleCoinSlotData& CoinSlot = CoinSlots.AddDefaulted_GetRef();
		CoinSlot.SlotNumber = SlotIndex + 1;
		CoinSlot.FrontWeaponID = SourceSlot.FrontWeaponID;
		CoinSlot.BackWeaponID = SourceSlot.BackWeaponID;
		CoinSlot.HP = ResolveCoinSlotHP(SourceSlot, DataManager);
		CoinSlot.AvailableCoinCount = SourceSlot.SameTypeCoinNum;
	}

	BroadcastCoinDataChanged();
}

void UCoinManagementWSubsystem::CreateTestCoinSlots()
{
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
	UDataManagerSubsystem* DataManager = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UDataManagerSubsystem>()
		: nullptr;

	if (!IsValid(DataManager) || !DataManager->IsCacheReady() || DataManager->WeaponByID.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CoinManager] 테스트 코인 생성 실패: DataManager 무기 데이터가 준비되지 않았습니다."));
		return;
	}

	for (const FTestCoinWeaponPair& WeaponPair : TestCoinWeaponPairs)
	{
		FFaceData FrontWeaponData;
		FFaceData BackWeaponData;
		if (!DataManager->TryGetWeapon(WeaponPair.FrontWeaponID, FrontWeaponData) ||
			!DataManager->TryGetWeapon(WeaponPair.BackWeaponID, BackWeaponData))
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[CoinManager] 테스트 코인 생성 실패: 고정 무기 ID를 찾지 못했습니다. Front=%d Back=%d"),
				WeaponPair.FrontWeaponID, WeaponPair.BackWeaponID);
			return;
		}
	}

	TArray<FCoinTypeStructure> TestCoinSlots;
	for (int32 SlotIndex = 0; SlotIndex < static_cast<int32>(UE_ARRAY_COUNT(TestCoinWeaponPairs)); ++SlotIndex)
	{
		const FTestCoinWeaponPair& WeaponPair = TestCoinWeaponPairs[SlotIndex];
		FCoinTypeStructure& TestCoinSlot = TestCoinSlots.AddDefaulted_GetRef();
		TestCoinSlot.SlotNum = SlotIndex + 1;
		TestCoinSlot.FrontWeaponID = WeaponPair.FrontWeaponID;
		TestCoinSlot.BackWeaponID = WeaponPair.BackWeaponID;
		TestCoinSlot.SameTypeCoinNum = 10;
		TestCoinSlot.Level = SlotIndex + 1;
	}

	InitializeCoinSlots(TestCoinSlots);
	bUsingTestCoinData = true;

	for (int32 SlotIndex = 0; SlotIndex < CoinSlots.Num(); ++SlotIndex)
	{
		// 테스트 코인이 스탯과 설명을 소유하고, PlayerController는 값을 전달만 합니다.
		CoinSlots[SlotIndex].FrontWeaponStats = MakeTestWeaponStatData(CoinSlots[SlotIndex].FrontWeaponID);
		CoinSlots[SlotIndex].BackWeaponStats = MakeTestWeaponStatData(CoinSlots[SlotIndex].BackWeaponID);
	}
	BroadcastCoinDataChanged();
}

bool UCoinManagementWSubsystem::TryAddReadyCoinFromSlot(int32 SlotNumber)
{
	if (!bIsCoinReadyPhase || GetReadyCoinCount() >= MaxReadyCoinCount)
	{
		return false;
	}

	FBattleCoinSlotData* CoinSlot = FindCoinSlot(SlotNumber);
	if (!CoinSlot || CoinSlot->AvailableCoinCount <= 0)
	{
		return false;
	}

	const int32 ReadyCoinSlotIndex = FindEmptyReadyCoinSlotIndex();
	const int32 CoinInstanceID = AllocateCoinInstanceID();
	if (ReadyCoinSlotIndex == INDEX_NONE || CoinInstanceID == INDEX_NONE)
	{
		return false;
	}

	FReadyCoinData& ReadyCoin = ReadyCoins[ReadyCoinSlotIndex];
	ReadyCoin = FReadyCoinData();
	ReadyCoin.CoinInstanceID = CoinInstanceID;
	ReadyCoin.SourceSlotNumber = CoinSlot->SlotNumber;
	ReadyCoin.FrontWeaponID = CoinSlot->FrontWeaponID;
	ReadyCoin.BackWeaponID = CoinSlot->BackWeaponID;
	ReadyCoin.CurrentHP = CoinSlot->HP;
	ReadyCoin.BaseMaxHP = CoinSlot->HP;
	ReadyCoin.Shield = 0;
	ReadyCoin.PersistentStatusEffects.Reset();
	ReadyCoin.bCanCancel = true;

	--CoinSlot->AvailableCoinCount;
	OnCoinAddedToReady.Broadcast();
	BroadcastCoinDataChanged();
	return true;
}

bool UCoinManagementWSubsystem::TryCancelReadyCoin(int32 CoinInstanceID)
{
	const int32 ReadyCoinIndex = ReadyCoins.IndexOfByPredicate([CoinInstanceID](const FReadyCoinData& ReadyCoin)
	{
		return ReadyCoin.CoinInstanceID == CoinInstanceID;
	});

	if (ReadyCoinIndex == INDEX_NONE || !ReadyCoins[ReadyCoinIndex].bCanCancel)
	{
		return false;
	}

	FBattleCoinSlotData* SourceCoinSlot = FindCoinSlot(ReadyCoins[ReadyCoinIndex].SourceSlotNumber);
	if (!SourceCoinSlot)
	{
		return false;
	}

	++SourceCoinSlot->AvailableCoinCount;
	ReadyCoins[ReadyCoinIndex] = FReadyCoinData();
	BroadcastCoinDataChanged();
	return true;
}

bool UCoinManagementWSubsystem::RemoveReadyCoinByInstanceID(int32 CoinInstanceID)
{
	const int32 ReadyCoinIndex = ReadyCoins.IndexOfByPredicate([CoinInstanceID](const FReadyCoinData& ReadyCoin)
	{
		return ReadyCoin.CoinInstanceID == CoinInstanceID;
	});

	if (ReadyCoinIndex == INDEX_NONE)
	{
		return false;
	}

	ReleaseAndDestroyRuntimeCoin(ReadyCoinIndex);

	// 사망한 코인만 비우며, 뒤의 레디 코인을 앞으로 당기지 않습니다.
	ReadyCoins[ReadyCoinIndex] = FReadyCoinData();
	BroadcastCoinDataChanged();
	return true;
}

void UCoinManagementWSubsystem::InitBattleReadyCoin()
{
	for (int32 ReadySlotIndex = 0; ReadySlotIndex < RuntimeCoinsByReadySlot.Num(); ++ReadySlotIndex)
	{
		ReleaseAndDestroyRuntimeCoin(ReadySlotIndex);
	}

	ReadyCoins.Init(FReadyCoinData(), MaxReadyCoinCount);
	BroadcastCoinDataChanged();
}

int32 UCoinManagementWSubsystem::InstantiateReadyCoinActors()
{
	// 생성 자산/DB가 잘못되어 특정 액터를 만들지 못하더라도 BehaviorPhase 진입 사실은 유지합니다.
	for (FReadyCoinData& ReadyCoin : ReadyCoins)
	{
		if (ReadyCoin.CoinInstanceID != INDEX_NONE)
		{
			ReadyCoin.bCanCancel = false;
		}
	}
	BroadcastCoinDataChanged();

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
	UDataManagerSubsystem* DataManager = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UDataManagerSubsystem>()
		: nullptr;
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();

	if (!IsValid(World) || !IsValid(DataManager) || !DataManager->IsCacheReady() || !IsValid(Settings))
	{
		UE_LOG(LogTemp, Error, TEXT("[CoinManager] CoinActor 생성 실패: World, DataManager 또는 Settings가 유효하지 않습니다."));
		return 0;
	}

	UClass* CoinClass = Settings->CoinActor.LoadSynchronous();
	if (!IsValid(CoinClass) || !CoinClass->IsChildOf(ACoinActor::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("[CoinManager] CoinActor 생성 실패: DeveloperSettings의 CoinActor 클래스가 유효하지 않습니다."));
		return 0;
	}

	if (RuntimeCoinsByReadySlot.Num() != MaxReadyCoinCount)
	{
		RuntimeCoinsByReadySlot.SetNum(MaxReadyCoinCount);
	}

	for (int32 ReadySlotIndex = 0; ReadySlotIndex < RuntimeCoinsByReadySlot.Num(); ++ReadySlotIndex)
	{
		ReleaseAndDestroyRuntimeCoin(ReadySlotIndex);
	}

	int32 SpawnedCoinCount = 0;
	for (int32 ReadySlotIndex = 0; ReadySlotIndex < ReadyCoins.Num(); ++ReadySlotIndex)
	{
		FReadyCoinData& ReadyCoin = ReadyCoins[ReadySlotIndex];
		if (ReadyCoin.CoinInstanceID == INDEX_NONE)
		{
			continue;
		}

		FFaceData FrontWeaponData;
		FFaceData BackWeaponData;
		if (!DataManager->TryGetWeapon(ReadyCoin.FrontWeaponID, FrontWeaponData) ||
			!DataManager->TryGetWeapon(ReadyCoin.BackWeaponID, BackWeaponData))
		{
			UE_LOG(LogTemp, Error,
				TEXT("[CoinManager] ReadySlot=%d CoinID=%d 생성 건너뜀: 무기 ID를 찾지 못했습니다. Front=%d Back=%d"),
				ReadySlotIndex + 1, ReadyCoin.CoinInstanceID, ReadyCoin.FrontWeaponID, ReadyCoin.BackWeaponID);
			continue;
		}

		if (!IsValid(FrontWeaponData.WeaponIcon) || !IsValid(BackWeaponData.WeaponIcon))
		{
			UE_LOG(LogTemp, Error,
				TEXT("[CoinManager] ReadySlot=%d CoinID=%d 생성 건너뜀: 앞/뒷면 무기 아이콘이 유효하지 않습니다."),
				ReadySlotIndex + 1, ReadyCoin.CoinInstanceID);
			continue;
		}

		const FTransform SpawnTransform(FRotator::ZeroRotator, FVector(0.0f, 0.0f, -10000.0f));
		ACoinActor* SpawnedCoin = World->SpawnActorDeferred<ACoinActor>(
			CoinClass,
			SpawnTransform,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		if (!IsValid(SpawnedCoin))
		{
			UE_LOG(LogTemp, Error, TEXT("[CoinManager] ReadySlot=%d CoinID=%d CoinActor Deferred Spawn 실패."),
				ReadySlotIndex + 1, ReadyCoin.CoinInstanceID);
			continue;
		}

		SpawnedCoin->SetActorHiddenInGame(true);
		SpawnedCoin->SetActorEnableCollision(false);

		FCoinStatInitializeData StatInitializeData;
		StatInitializeData.FrontWeaponStats = BuildTemporaryWeaponFaceStats(FrontWeaponData);
		StatInitializeData.BackWeaponStats = BuildTemporaryWeaponFaceStats(BackWeaponData);
		StatInitializeData.RuntimeState.BaseMaxHP = ReadyCoin.BaseMaxHP > 0
			? ReadyCoin.BaseMaxHP
			: ReadyCoin.CurrentHP;
		StatInitializeData.RuntimeState.CurrentHP = ReadyCoin.CurrentHP;
		StatInitializeData.RuntimeState.Shield = ReadyCoin.Shield;
		StatInitializeData.RuntimeState.PersistentStatusEffects = ReadyCoin.PersistentStatusEffects;

		const bool bInitialized = SpawnedCoin->SetCoinValues(
			ReadyCoin.CoinInstanceID,
			ReadyCoin.FrontWeaponID,
			ReadyCoin.BackWeaponID,
			FrontWeaponData.WeaponType,
			FrontWeaponData.WeaponIcon,
			BackWeaponData.WeaponIcon,
			StatInitializeData
		);

		SpawnedCoin->SetCoinIsReady(false);
		SpawnedCoin->SetCoinOnBattle(true);
		SpawnedCoin->FinishSpawning(SpawnTransform);

		if (!bInitialized || !IsValid(SpawnedCoin))
		{
			UE_LOG(LogTemp, Error, TEXT("[CoinManager] ReadySlot=%d CoinID=%d 초기화 실패."),
				ReadySlotIndex + 1, ReadyCoin.CoinInstanceID);
			if (IsValid(SpawnedCoin))
			{
				SpawnedCoin->Destroy();
			}
			continue;
		}

		SpawnedCoin->SetActorHiddenInGame(true);
		SpawnedCoin->SetActorEnableCollision(false);
		SpawnedCoin->SetWeaponDefinitions(FrontWeaponData, BackWeaponData);
		SpawnedCoin->OnCoinDeathStarted.AddUObject(this, &UCoinManagementWSubsystem::HandleRuntimeCoinDeathStarted);
		BindRuntimeCoinInteraction(SpawnedCoin);
		RuntimeCoinsByReadySlot[ReadySlotIndex] = SpawnedCoin;
		++SpawnedCoinCount;
	}

	BroadcastCoinDataChanged();
	return SpawnedCoinCount;
}

void UCoinManagementWSubsystem::CheckBattleReadyCoinAlive()
{
	for (int32 ReadySlotIndex = 0; ReadySlotIndex < RuntimeCoinsByReadySlot.Num(); ++ReadySlotIndex)
	{
		ACoinActor* RuntimeCoin = RuntimeCoinsByReadySlot[ReadySlotIndex].Get();
		if (IsValid(RuntimeCoin) && ReadyCoins.IsValidIndex(ReadySlotIndex))
		{
			UComponent_Status* StatusComponent = RuntimeCoin->StatComponent;
			if (IsValid(StatusComponent))
			{
				// SettingPhase에서는 방금 끝난 턴의 효과를 먼저 버리고 전투 지속 효과만 저장합니다.
				StatusComponent->RemoveTurnOnlyStatusEffects();
				StatusComponent->AdvancePersistentStatusEffectsAtTurnEnd();
			}
			const FCoinRuntimeStateSnapshot RuntimeState = IsValid(StatusComponent)
				? StatusComponent->ExportRuntimeState()
				: FCoinRuntimeStateSnapshot();

			if (IsValid(StatusComponent) && RuntimeState.CurrentHP > 0 &&
				ReadyCoins[ReadySlotIndex].CoinInstanceID != INDEX_NONE)
			{
				ReadyCoins[ReadySlotIndex].CurrentHP = RuntimeState.CurrentHP;
				ReadyCoins[ReadySlotIndex].BaseMaxHP = RuntimeState.BaseMaxHP;
				ReadyCoins[ReadySlotIndex].Shield = RuntimeState.Shield;
				ReadyCoins[ReadySlotIndex].PersistentStatusEffects = RuntimeState.PersistentStatusEffects;
				ReadyCoins[ReadySlotIndex].bCanCancel = false;
			}
			else
			{
				ReadyCoins[ReadySlotIndex] = FReadyCoinData();
			}
		}

		ReleaseAndDestroyRuntimeCoin(ReadySlotIndex);
	}

	BroadcastCoinDataChanged();
}

TArray<ACoinActor*> UCoinManagementWSubsystem::GetReadyCoins() const
{
	TArray<ACoinActor*> RuntimeCoins;
	RuntimeCoins.Reserve(RuntimeCoinsByReadySlot.Num());
	for (const TObjectPtr<ACoinActor>& RuntimeCoinPtr : RuntimeCoinsByReadySlot)
	{
		ACoinActor* RuntimeCoin = RuntimeCoinPtr.Get();
		if (IsValid(RuntimeCoin))
		{
			RuntimeCoins.Add(RuntimeCoin);
		}
	}
	return RuntimeCoins;
}

ACoinActor* UCoinManagementWSubsystem::GetRuntimeCoinAtReadySlot(int32 ReadySlotIndex) const
{
	return RuntimeCoinsByReadySlot.IsValidIndex(ReadySlotIndex)
		? RuntimeCoinsByReadySlot[ReadySlotIndex].Get()
		: nullptr;
}

void UCoinManagementWSubsystem::SetBattleCoinItemFlags(bool bEnabled)
{
	for (const TObjectPtr<ACoinActor>& RuntimeCoinPtr : RuntimeCoinsByReadySlot)
	{
		ACoinActor* RuntimeCoin = RuntimeCoinPtr.Get();
		if (IsValid(RuntimeCoin))
		{
			RuntimeCoin->SetCoinItemFlag(bEnabled);
		}
	}
}

bool UCoinManagementWSubsystem::IsCoinInBattleReady(ACoinActor* InCoin) const
{
	return IsValid(InCoin) && RuntimeCoinsByReadySlot.ContainsByPredicate([InCoin](const TObjectPtr<ACoinActor>& RuntimeCoin)
	{
		return RuntimeCoin.Get() == InCoin;
	});
}

bool UCoinManagementWSubsystem::IsCoinIdInBattleReady(int32 TargetID) const
{
	return ReadyCoins.ContainsByPredicate([TargetID](const FReadyCoinData& ReadyCoin)
	{
		return ReadyCoin.CoinInstanceID == TargetID;
	});
}

void UCoinManagementWSubsystem::LockCoinReady(ACoinActor* TargetCoin)
{
	if (!IsValid(TargetCoin))
	{
		return;
	}

	if (FReadyCoinData* ReadyCoin = ReadyCoins.FindByPredicate([TargetCoin](const FReadyCoinData& Data)
	{
		return Data.CoinInstanceID == TargetCoin->GetCoinID();
	}))
	{
		ReadyCoin->bCanCancel = false;
		BroadcastCoinDataChanged();
	}
}

int32 UCoinManagementWSubsystem::CalculateCoinPrice() const
{
	return 0;
}

int32 UCoinManagementWSubsystem::CalculateCoinCount() const
{
	int32 TotalCoinCount = GetReadyCoinCount();
	for (const FBattleCoinSlotData& CoinSlot : CoinSlots)
	{
		TotalCoinCount += CoinSlot.AvailableCoinCount;
	}
	return TotalCoinCount;
}

int32 UCoinManagementWSubsystem::FindEmptyReadyCoinSlotIndex() const
{
	return ReadyCoins.IndexOfByPredicate([](const FReadyCoinData& ReadyCoin)
	{
		return ReadyCoin.CoinInstanceID == INDEX_NONE;
	});
}

int32 UCoinManagementWSubsystem::AllocateCoinInstanceID() const
{
	for (int32 CandidateID = 1; CandidateID <= MaxReadyCoinCount; ++CandidateID)
	{
		const bool bAlreadyUsed = ReadyCoins.ContainsByPredicate([CandidateID](const FReadyCoinData& ReadyCoin)
		{
			return ReadyCoin.CoinInstanceID == CandidateID;
		});

		if (!bAlreadyUsed)
		{
			return CandidateID;
		}
	}

	return INDEX_NONE;
}

int32 UCoinManagementWSubsystem::GetReadyCoinCount() const
{
	int32 ReadyCoinCount = 0;
	for (const FReadyCoinData& ReadyCoin : ReadyCoins)
	{
		if (ReadyCoin.CoinInstanceID != INDEX_NONE)
		{
			++ReadyCoinCount;
		}
	}

	return ReadyCoinCount;
}

FBattleCoinSlotData* UCoinManagementWSubsystem::FindCoinSlot(int32 SlotNumber)
{
	return CoinSlots.FindByPredicate([SlotNumber](const FBattleCoinSlotData& CoinSlot)
	{
		return CoinSlot.SlotNumber == SlotNumber;
	});
}

const FBattleCoinSlotData* UCoinManagementWSubsystem::FindCoinSlot(int32 SlotNumber) const
{
	return CoinSlots.FindByPredicate([SlotNumber](const FBattleCoinSlotData& CoinSlot)
	{
		return CoinSlot.SlotNumber == SlotNumber;
	});
}

int32 UCoinManagementWSubsystem::ResolveCoinSlotHP(
	const FCoinTypeStructure& CoinSlot,
	const UDataManagerSubsystem* DataManager) const
{
	int32 Cost = 0;
	int32 HP = 0;
	if (IsValid(DataManager) && DataManager->GetCoinSlotLevelStats(CoinSlot, Cost, HP) && HP > 0)
	{
		return HP;
	}

	const int32 FallbackHP = FMath::Clamp(CoinSlot.Level, 1, 3) * 5;
	UE_LOG(LogTemp, Warning,
		TEXT("[CoinManager] 슬롯 레벨 HP 조회 실패: Level=%d, fallback HP=%d를 사용합니다."),
		CoinSlot.Level, FallbackHP);
	return FallbackHP;
}

FWeaponFaceStats UCoinManagementWSubsystem::BuildTemporaryWeaponFaceStats(const FFaceData& LegacyFaceData) const
{
	if (bUsingTestCoinData)
	{
		return MakeTestWeaponFaceStats(LegacyFaceData.WeaponID);
	}

	FWeaponFaceStats FaceStats;
	FaceStats.WeaponID = LegacyFaceData.WeaponID;
	FaceStats.BaseNumericStats.AttackPoint = FMath::Max(0, LegacyFaceData.AttackPoint);
	FaceStats.BaseNumericStats.WeaponPoint = FMath::Max(0, LegacyFaceData.BehaviorPoint);
	// DB에 횟수 필드가 추가되기 전에는 기획대로 0으로 표시합니다.
	FaceStats.BaseNumericStats.WeaponCnt = 0;
	FaceStats.AttackAreaSpec = LegacyFaceData.AttackAreaSpec;
	// TODO(DB_ABILITY_AREA_RECONNECT): DataManager가 능력 사거리 컬럼을 읽기 시작하면 이 값이 채워집니다.
	FaceStats.AbilityAreaSpec = LegacyFaceData.AbilityAreaSpec;
	FaceStats.bHasAbilityArea = false;
	return FaceStats;
}

void UCoinManagementWSubsystem::BroadcastCoinDataChanged()
{
	OnBattleCoinDataChanged.Broadcast();
}

void UCoinManagementWSubsystem::BindRuntimeCoinInteraction(ACoinActor* RuntimeCoin)
{
	if (!IsValid(RuntimeCoin) || !IsValid(CoinActionManager))
	{
		return;
	}

	RuntimeCoin->OnHoverBattleCoin.AddUniqueDynamic(
		CoinActionManager, &UCoinActionManagementWSubsystem::SetSelectedWeapon);
	RuntimeCoin->OnClickBattleCoin.AddUniqueDynamic(
		CoinActionManager, &UCoinActionManagementWSubsystem::ExecuteSelectedWeapon);
	RuntimeCoin->OnUnhoverCoin.AddUniqueDynamic(
		CoinActionManager, &UCoinActionManagementWSubsystem::HandleCoinUnHovered);
	RuntimeCoin->OnCoinRightClicked.AddUniqueDynamic(
		CoinActionManager, &UCoinActionManagementWSubsystem::CancelSingleCellAction);
}

void UCoinManagementWSubsystem::UnbindRuntimeCoinInteraction(ACoinActor* RuntimeCoin)
{
	if (!IsValid(RuntimeCoin) || !IsValid(CoinActionManager))
	{
		return;
	}

	RuntimeCoin->OnHoverBattleCoin.RemoveDynamic(
		CoinActionManager, &UCoinActionManagementWSubsystem::SetSelectedWeapon);
	RuntimeCoin->OnClickBattleCoin.RemoveDynamic(
		CoinActionManager, &UCoinActionManagementWSubsystem::ExecuteSelectedWeapon);
	RuntimeCoin->OnUnhoverCoin.RemoveDynamic(
		CoinActionManager, &UCoinActionManagementWSubsystem::HandleCoinUnHovered);
	RuntimeCoin->OnCoinRightClicked.RemoveDynamic(
		CoinActionManager, &UCoinActionManagementWSubsystem::CancelSingleCellAction);
}

void UCoinManagementWSubsystem::HandleRuntimeCoinDeathStarted(ACoinActor* DeadCoin)
{
	if (!IsValid(DeadCoin))
	{
		return;
	}

	const int32 ReadySlotIndex = RuntimeCoinsByReadySlot.IndexOfByKey(DeadCoin);
	if (ReadySlotIndex == INDEX_NONE)
	{
		return;
	}

	if (IsValid(GridManager))
	{
		GridManager->ReleaseCoinCell(DeadCoin->GetDecidedGrid(), DeadCoin);
	}

	UnbindRuntimeCoinInteraction(DeadCoin);
	DeadCoin->OnCoinDeathStarted.RemoveAll(this);
	RuntimeCoinsByReadySlot[ReadySlotIndex] = nullptr;
	if (ReadyCoins.IsValidIndex(ReadySlotIndex))
	{
		// 사망한 칸만 비우며 다른 ReadyCoin은 절대 재정렬하지 않습니다.
		ReadyCoins[ReadySlotIndex] = FReadyCoinData();
	}

	BroadcastCoinDataChanged();
	if (GetReadyCoinCount() == 0)
	{
		OnAllCoinDead.ExecuteIfBound();
	}
}

void UCoinManagementWSubsystem::ReleaseAndDestroyRuntimeCoin(int32 ReadySlotIndex)
{
	if (!RuntimeCoinsByReadySlot.IsValidIndex(ReadySlotIndex))
	{
		return;
	}

	ACoinActor* RuntimeCoin = RuntimeCoinsByReadySlot[ReadySlotIndex].Get();
	RuntimeCoinsByReadySlot[ReadySlotIndex] = nullptr;
	if (!IsValid(RuntimeCoin))
	{
		return;
	}

	if (IsValid(GridManager))
	{
		GridManager->ReleaseCoinCell(RuntimeCoin->GetDecidedGrid(), RuntimeCoin);
	}

	UnbindRuntimeCoinInteraction(RuntimeCoin);
	RuntimeCoin->OnCoinDeathStarted.RemoveAll(this);
	RuntimeCoin->Destroy();
}
