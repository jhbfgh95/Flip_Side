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
	struct FDummyCoinWeaponPair
	{
		int32 FrontWeaponID;
		int32 BackWeaponID;
	};

	constexpr FDummyCoinWeaponPair DummyCoinWeaponPairs[] =
	{
		{ 1, 2 },
		{ 3, 4 },
		{ 5, 6 },
		{ 7, 8 },
		{ 9, 10 },
		{ 11, 12 },
		{ 13, 14 },
		{ 15, 16 },
		{ 17, -2 },
		{ -3, 20 }
	};

	FWeaponStatDisplayData BuildWeaponStatDisplayData(const FWeaponFaceStats& FaceStats)
	{
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
	if (IsValid(CrossingLevelSubsystem))
	{
		// 중간 슬롯이 비어 있어도 뒤 슬롯을 잃지 않도록 고정된 10칸을 모두 확인합니다.
		for (int32 SlotIndex = 0; SlotIndex < MaxReadyCoinCount; ++SlotIndex)
		{
			const FCoinTypeStructure CoinSlot = CrossingLevelSubsystem->GetSlotCoin(SlotIndex);
			if (CoinSlot.SameTypeCoinNum > 0)
			{
				InventoryCoinSlots.Add(CoinSlot);
			}
		}
	}

	if (InventoryCoinSlots.IsEmpty())
	{
		CreateDummyCoinSlots();
	}
	else
	{
		InitializeCoinSlots(InventoryCoinSlots);
	}
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
	CoinSlots.Reset();
	ReadyCoins.Init(FReadyCoinData(), MaxReadyCoinCount);

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
	const UDataManagerSubsystem* DataManager = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UDataManagerSubsystem>()
		: nullptr;
	if (!IsValid(DataManager) || !DataManager->IsCacheReady())
	{
		UE_LOG(LogTemp, Error, TEXT("[CoinManager] 코인 슬롯 초기화 실패: DataManager 캐시가 준비되지 않았습니다."));
		BroadcastCoinDataChanged();
		return;
	}

	for (int32 SlotIndex = 0; SlotIndex < InCoinSlots.Num(); ++SlotIndex)
	{
		const FCoinTypeStructure& SourceSlot = InCoinSlots[SlotIndex];
		if (SourceSlot.SameTypeCoinNum <= 0)
		{
			continue;
		}

		FFaceData FrontWeaponData;
		FFaceData BackWeaponData;
		if (!DataManager->TryGetWeapon(SourceSlot.FrontWeaponID, FrontWeaponData) ||
			!DataManager->TryGetWeapon(SourceSlot.BackWeaponID, BackWeaponData))
		{
			UE_LOG(LogTemp, Error,
				TEXT("[CoinManager] 코인 슬롯 제외: DB 무기 조회 실패. Slot=%d Front=%d Back=%d"),
				SourceSlot.SlotNum + 1, SourceSlot.FrontWeaponID, SourceSlot.BackWeaponID);
			continue;
		}
		int32 SlotHP = 0;
		if (!ResolveCoinSlotHP(SourceSlot, DataManager, SlotHP))
		{
			continue;
		}

		const int32 DisplaySlotNumber = SourceSlot.SlotNum >= 0
			? SourceSlot.SlotNum + 1
			: SlotIndex + 1;
		if (FindCoinSlot(DisplaySlotNumber))
		{
			UE_LOG(LogTemp, Error,
				TEXT("[CoinManager] 코인 슬롯 제외: 중복 슬롯 번호입니다. Slot=%d"),
				DisplaySlotNumber);
			continue;
		}

		const FWeaponFaceStats FrontFaceStats = BuildWeaponFaceStatsFromDefinition(FrontWeaponData);
		const FWeaponFaceStats BackFaceStats = BuildWeaponFaceStatsFromDefinition(BackWeaponData);
		FBattleCoinSlotData& CoinSlot = CoinSlots.AddDefaulted_GetRef();
		CoinSlot.SlotNumber = DisplaySlotNumber;
		CoinSlot.FrontWeaponID = SourceSlot.FrontWeaponID;
		CoinSlot.BackWeaponID = SourceSlot.BackWeaponID;
		CoinSlot.HP = SlotHP;
		CoinSlot.AvailableCoinCount = SourceSlot.SameTypeCoinNum;
		CoinSlot.FrontWeaponStats = BuildWeaponStatDisplayData(FrontFaceStats);
		CoinSlot.BackWeaponStats = BuildWeaponStatDisplayData(BackFaceStats);
	}

	BroadcastCoinDataChanged();
}

void UCoinManagementWSubsystem::CreateDummyCoinSlots()
{
	TArray<FCoinTypeStructure> DummyCoinSlots;
	DummyCoinSlots.Reserve(UE_ARRAY_COUNT(DummyCoinWeaponPairs));

	for (int32 SlotIndex = 0; SlotIndex < UE_ARRAY_COUNT(DummyCoinWeaponPairs); ++SlotIndex)
	{
		const FDummyCoinWeaponPair& WeaponPair = DummyCoinWeaponPairs[SlotIndex];
		FCoinTypeStructure& DummySlot = DummyCoinSlots.AddDefaulted_GetRef();
		DummySlot.SlotNum = SlotIndex;
		DummySlot.FrontWeaponID = WeaponPair.FrontWeaponID;
		DummySlot.BackWeaponID = WeaponPair.BackWeaponID;
		DummySlot.SameTypeCoinNum = 3;
		DummySlot.Level = 1;
	}

	// 더미는 ID와 슬롯 기본값만 소유하며, 스탯·HP·아이콘·사거리는 실제 DB 초기화 경로에서 조회합니다.
	InitializeCoinSlots(DummyCoinSlots);
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

bool UCoinManagementWSubsystem::ReplaceReadyCoinWithSample(
	int32 FrontWeaponID,
	int32 BackWeaponID,
	int32 ReadySlotNumber)
{
	const int32 ReadySlotIndex = ReadySlotNumber - 1;
	if (!ReadyCoins.IsValidIndex(ReadySlotIndex))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[CoinManager] CreateSampleCoin 실패: ReadySlot은 1~10이어야 합니다."));
		return false;
	}

	const FReadyCoinData PreviousReadyCoin = ReadyCoins[ReadySlotIndex];
	if (PreviousReadyCoin.CoinInstanceID == INDEX_NONE || PreviousReadyCoin.BaseMaxHP <= 0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[CoinManager] CreateSampleCoin 실패: ReadySlot=%d에 교체할 코인이 없습니다."),
			ReadySlotNumber);
		return false;
	}
	if (IsValid(CoinActionManager) && CoinActionManager->IsActionSequenceActive())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[CoinManager] CreateSampleCoin 실패: 코인 행동 진행 중에는 교체할 수 없습니다."));
		return false;
	}

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
	UDataManagerSubsystem* DataManager = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UDataManagerSubsystem>()
		: nullptr;
	FFaceData FrontWeaponData;
	FFaceData BackWeaponData;
	if (!IsValid(DataManager) || !DataManager->IsCacheReady() ||
		!DataManager->TryGetWeapon(FrontWeaponID, FrontWeaponData) ||
		!DataManager->TryGetWeapon(BackWeaponID, BackWeaponData))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[CoinManager] CreateSampleCoin 실패: DB 무기 조회 실패. Front=%d Back=%d"),
			FrontWeaponID, BackWeaponID);
		return false;
	}
	if (FrontWeaponData.WeaponType == EWeaponClass::None ||
		!IsValid(FrontWeaponData.WeaponIcon) || !IsValid(BackWeaponData.WeaponIcon))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[CoinManager] CreateSampleCoin 실패: 앞면 무기 클래스 또는 앞/뒷면 아이콘이 유효하지 않습니다."));
		return false;
	}

	if (RuntimeCoinsByReadySlot.Num() != MaxReadyCoinCount)
	{
		RuntimeCoinsByReadySlot.SetNum(MaxReadyCoinCount);
	}
	ACoinActor* PreviousRuntimeCoin = RuntimeCoinsByReadySlot[ReadySlotIndex].Get();
	if (IsValid(PreviousRuntimeCoin) && !IsValid(GridManager))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[CoinManager] CreateSampleCoin 실패: 필드 코인 교체에 필요한 GridManager가 유효하지 않습니다."));
		return false;
	}

	FReadyCoinData ReplacementCoin;
	ReplacementCoin.CoinInstanceID = PreviousReadyCoin.CoinInstanceID;
	ReplacementCoin.SourceSlotNumber = INDEX_NONE;
	ReplacementCoin.FrontWeaponID = FrontWeaponID;
	ReplacementCoin.BackWeaponID = BackWeaponID;
	ReplacementCoin.CurrentHP = PreviousReadyCoin.BaseMaxHP;
	ReplacementCoin.BaseMaxHP = PreviousReadyCoin.BaseMaxHP;
	ReplacementCoin.Shield = 0;
	ReplacementCoin.PersistentStatusEffects.Reset();
	ReplacementCoin.bCanCancel = false;

	if (!IsValid(PreviousRuntimeCoin))
	{
		ReadyCoins[ReadySlotIndex] = MoveTemp(ReplacementCoin);
		BroadcastCoinDataChanged();
		return true;
	}

	if (IsValid(CoinActionManager))
	{
		CoinActionManager->CancelSelectWeapon();
	}

	const FTransform PreviousTransform = PreviousRuntimeCoin->GetActorTransform();
	const bool bWasHidden = PreviousRuntimeCoin->IsHidden();
	const bool bHadCollision = PreviousRuntimeCoin->GetActorEnableCollision();
	const EFaceState PreviousFace = PreviousRuntimeCoin->GetCoinDecidedFace();
	const FGridPoint PreviousGrid = PreviousRuntimeCoin->GetDecidedGrid();
	// BattleManager는 그리드 점유 성공 뒤에만 면을 결정하므로 None 여부로 배치 전/후를 구분합니다.
	const bool bHadGridOccupation = PreviousFace != EFaceState::None;

	ReadyCoins[ReadySlotIndex] = ReplacementCoin;
	ACoinActor* ReplacementRuntimeCoin = SpawnRuntimeCoinActor(
		ReadySlotIndex, PreviousTransform, bWasHidden, false);
	if (!IsValid(ReplacementRuntimeCoin))
	{
		ReadyCoins[ReadySlotIndex] = PreviousReadyCoin;
		return false;
	}

	ReplacementRuntimeCoin->SetGridPoint(PreviousGrid);
	if (PreviousFace != EFaceState::None)
	{
		ReplacementRuntimeCoin->SetCoinFace(PreviousFace);
	}
	ReplacementRuntimeCoin->SetCoinIsActed(false);
	ReplacementRuntimeCoin->SetCoinIsActing(false);

	if (bHadGridOccupation)
	{
		GridManager->ReleaseCoinCell(PreviousGrid, PreviousRuntimeCoin);
		if (!GridManager->TryOccupyCoinCell(PreviousGrid, ReplacementRuntimeCoin))
		{
			ReplacementRuntimeCoin->Destroy();
			ReadyCoins[ReadySlotIndex] = PreviousReadyCoin;
			GridManager->TryOccupyCoinCell(PreviousGrid, PreviousRuntimeCoin);
			UE_LOG(LogTemp, Error,
				TEXT("[CoinManager] CreateSampleCoin 실패: ReadySlot=%d의 그리드 점유 교체에 실패했습니다."),
				ReadySlotNumber);
			return false;
		}
	}

	UnbindRuntimeCoinInteraction(PreviousRuntimeCoin);
	PreviousRuntimeCoin->OnCoinDeathStarted.RemoveAll(this);
	PreviousRuntimeCoin->Destroy();

	ReplacementRuntimeCoin->SetActorHiddenInGame(bWasHidden);
	ReplacementRuntimeCoin->SetActorEnableCollision(bHadCollision);
	ReplacementRuntimeCoin->OnCoinDeathStarted.AddUObject(
		this, &UCoinManagementWSubsystem::HandleRuntimeCoinDeathStarted);
	BindRuntimeCoinInteraction(ReplacementRuntimeCoin);
	RuntimeCoinsByReadySlot[ReadySlotIndex] = ReplacementRuntimeCoin;
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
		const FReadyCoinData& ReadyCoin = ReadyCoins[ReadySlotIndex];
		if (ReadyCoin.CoinInstanceID == INDEX_NONE)
		{
			continue;
		}

		const FTransform SpawnTransform(FRotator::ZeroRotator, FVector(0.0f, 0.0f, -10000.0f));
		ACoinActor* SpawnedCoin = SpawnRuntimeCoinActor(
			ReadySlotIndex, SpawnTransform, true, false);
		if (!IsValid(SpawnedCoin))
		{
			continue;
		}

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

bool UCoinManagementWSubsystem::ResolveCoinSlotHP(
	const FCoinTypeStructure& CoinSlot,
	const UDataManagerSubsystem* DataManager,
	int32& OutHP) const
{
	OutHP = 0;
	int32 Cost = 0;
	int32 HP = 0;
	if (IsValid(DataManager) && DataManager->GetCoinSlotLevelStats(CoinSlot, Cost, HP) && HP > 0)
	{
		OutHP = HP;
		return true;
	}

	UE_LOG(LogTemp, Error,
		TEXT("[CoinManager] 슬롯 레벨 HP 조회 실패: Level=%d. 하드코딩 fallback을 사용하지 않습니다."),
		CoinSlot.Level);
	return false;
}

ACoinActor* UCoinManagementWSubsystem::SpawnRuntimeCoinActor(
	int32 ReadySlotIndex,
	const FTransform& SpawnTransform,
	bool bHiddenInGame,
	bool bCollisionEnabled)
{
	if (!ReadyCoins.IsValidIndex(ReadySlotIndex))
	{
		return nullptr;
	}

	const FReadyCoinData& ReadyCoin = ReadyCoins[ReadySlotIndex];
	if (ReadyCoin.CoinInstanceID == INDEX_NONE || ReadyCoin.CurrentHP <= 0)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? World->GetGameInstance() : nullptr;
	UDataManagerSubsystem* DataManager = IsValid(GameInstance)
		? GameInstance->GetSubsystem<UDataManagerSubsystem>()
		: nullptr;
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!IsValid(World) || !IsValid(DataManager) || !DataManager->IsCacheReady() || !IsValid(Settings))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[CoinManager] ReadySlot=%d CoinActor 생성 실패: World, DataManager 또는 Settings가 유효하지 않습니다."),
			ReadySlotIndex + 1);
		return nullptr;
	}

	FFaceData FrontWeaponData;
	FFaceData BackWeaponData;
	if (!DataManager->TryGetWeapon(ReadyCoin.FrontWeaponID, FrontWeaponData) ||
		!DataManager->TryGetWeapon(ReadyCoin.BackWeaponID, BackWeaponData))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[CoinManager] ReadySlot=%d CoinID=%d 생성 실패: DB 무기 조회 실패. Front=%d Back=%d"),
			ReadySlotIndex + 1, ReadyCoin.CoinInstanceID,
			ReadyCoin.FrontWeaponID, ReadyCoin.BackWeaponID);
		return nullptr;
	}
	if (!IsValid(FrontWeaponData.WeaponIcon) || !IsValid(BackWeaponData.WeaponIcon) ||
		FrontWeaponData.WeaponType == EWeaponClass::None)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[CoinManager] ReadySlot=%d CoinID=%d 생성 실패: 앞면 무기 클래스 또는 앞/뒷면 아이콘이 유효하지 않습니다."),
			ReadySlotIndex + 1, ReadyCoin.CoinInstanceID);
		return nullptr;
	}

	UClass* CoinClass = Settings->CoinActor.LoadSynchronous();
	if (!IsValid(CoinClass) || !CoinClass->IsChildOf(ACoinActor::StaticClass()))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[CoinManager] ReadySlot=%d CoinActor 생성 실패: DeveloperSettings의 CoinActor 클래스가 유효하지 않습니다."),
			ReadySlotIndex + 1);
		return nullptr;
	}

	ACoinActor* SpawnedCoin = World->SpawnActorDeferred<ACoinActor>(
		CoinClass,
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!IsValid(SpawnedCoin))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[CoinManager] ReadySlot=%d CoinID=%d Deferred Spawn 실패."),
			ReadySlotIndex + 1, ReadyCoin.CoinInstanceID);
		return nullptr;
	}

	SpawnedCoin->SetActorHiddenInGame(bHiddenInGame);
	SpawnedCoin->SetActorEnableCollision(bCollisionEnabled);

	FCoinStatInitializeData StatInitializeData;
	StatInitializeData.FrontWeaponStats = BuildWeaponFaceStatsFromDefinition(FrontWeaponData);
	StatInitializeData.BackWeaponStats = BuildWeaponFaceStatsFromDefinition(BackWeaponData);
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
		StatInitializeData);
	SpawnedCoin->SetCoinIsReady(false);
	SpawnedCoin->SetCoinOnBattle(true);
	SpawnedCoin->FinishSpawning(SpawnTransform);

	if (!bInitialized || !IsValid(SpawnedCoin))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[CoinManager] ReadySlot=%d CoinID=%d 초기화 실패."),
			ReadySlotIndex + 1, ReadyCoin.CoinInstanceID);
		if (IsValid(SpawnedCoin))
		{
			SpawnedCoin->Destroy();
		}
		return nullptr;
	}

	SpawnedCoin->SetActorHiddenInGame(bHiddenInGame);
	SpawnedCoin->SetActorEnableCollision(bCollisionEnabled);
	SpawnedCoin->SetWeaponDefinitions(FrontWeaponData, BackWeaponData);
	return SpawnedCoin;
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
