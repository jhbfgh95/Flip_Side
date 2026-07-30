#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"

#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystems/Subsystem.h"

void UCoinManagementWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
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
	CoinSlots.Reset();
	ReadyCoins.Reset();
	NextCoinInstanceID = 1;

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
		CoinSlot.HP = GetHPForSlotIndex(SlotIndex);
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

	TArray<int32> WeaponIDs;
	DataManager->WeaponByID.GenerateKeyArray(WeaponIDs);
	if (WeaponIDs.IsEmpty())
	{
		return;
	}

	TArray<FCoinTypeStructure> TestCoinSlots;
	for (int32 SlotIndex = 0; SlotIndex < 3; ++SlotIndex)
	{
		FCoinTypeStructure& TestCoinSlot = TestCoinSlots.AddDefaulted_GetRef();
		TestCoinSlot.SlotNum = SlotIndex + 1;
		TestCoinSlot.FrontWeaponID = WeaponIDs[FMath::RandRange(0, WeaponIDs.Num() - 1)];
		TestCoinSlot.BackWeaponID = WeaponIDs[FMath::RandRange(0, WeaponIDs.Num() - 1)];
		TestCoinSlot.SameTypeCoinNum = 10;
	}

	InitializeCoinSlots(TestCoinSlots);
}

bool UCoinManagementWSubsystem::TryAddReadyCoinFromSlot(int32 SlotNumber)
{
	if (!bIsCoinReadyTurn || ReadyCoins.Num() >= MaxReadyCoinCount)
	{
		return false;
	}

	FBattleCoinSlotData* CoinSlot = FindCoinSlot(SlotNumber);
	if (!CoinSlot || CoinSlot->AvailableCoinCount <= 0)
	{
		return false;
	}

	FReadyCoinData& ReadyCoin = ReadyCoins.AddDefaulted_GetRef();
	ReadyCoin.CoinInstanceID = NextCoinInstanceID++;
	ReadyCoin.SourceSlotNumber = CoinSlot->SlotNumber;
	ReadyCoin.FrontWeaponID = CoinSlot->FrontWeaponID;
	ReadyCoin.BackWeaponID = CoinSlot->BackWeaponID;
	ReadyCoin.CurrentHP = CoinSlot->HP;
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
	ReadyCoins.RemoveAt(ReadyCoinIndex);
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

	ReadyCoins.RemoveAt(ReadyCoinIndex);
	BroadcastCoinDataChanged();
	return true;
}

void UCoinManagementWSubsystem::InitBattleReadyCoin()
{
	ReadyCoins.Reset();
	BroadcastCoinDataChanged();
}

void UCoinManagementWSubsystem::CheckBattleReadyCoinAlive()
{
	// TODO: CoinBehaviorTurn에서 살아남은 CoinActor를 ReadyCoinData로 복귀시키는 흐름을 구현합니다.
}

void UCoinManagementWSubsystem::AddBattleReadyCoins(ACoinActor* SelectCoinActor, bool bArrangeSlot)
{
	// TODO: 기존 3D ReadyCoin 흐름은 CoinBehaviorTurn 리팩터링 시 제거합니다.
}

void UCoinManagementWSubsystem::RemoveBattleReadyCoins(ACoinActor* SelectCoinActor)
{
	// TODO: 기존 3D ReadyCoin 흐름은 CoinBehaviorTurn 리팩터링 시 제거합니다.
}

TArray<ACoinActor*> UCoinManagementWSubsystem::GetReadyCoins() const
{
	// 기존 Actor 기반 호출부와의 호환용입니다. 이번 단계에서는 CoinActor를 생성하지 않습니다.
	return {};
}

void UCoinManagementWSubsystem::SetBattleCoinItemFlags(bool bEnabled)
{
	// CoinBehaviorTurn 전까지 전투 CoinActor가 존재하지 않습니다.
}

bool UCoinManagementWSubsystem::IsCoinInBattleReady(ACoinActor* InCoin) const
{
	return false;
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
	// CoinBehaviorTurn 전까지 전투 CoinActor가 존재하지 않습니다.
}

int32 UCoinManagementWSubsystem::CalculateCoinPrice() const
{
	return 0;
}

int32 UCoinManagementWSubsystem::CalculateCoinCount() const
{
	int32 TotalCoinCount = ReadyCoins.Num();
	for (const FBattleCoinSlotData& CoinSlot : CoinSlots)
	{
		TotalCoinCount += CoinSlot.AvailableCoinCount;
	}
	return TotalCoinCount;
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

int32 UCoinManagementWSubsystem::GetHPForSlotIndex(int32 SlotIndex) const
{
	return FMath::Clamp((SlotIndex + 1) * 5, 5, 15);
}

void UCoinManagementWSubsystem::BroadcastCoinDataChanged()
{
	OnBattleCoinDataChanged.Broadcast();
}
