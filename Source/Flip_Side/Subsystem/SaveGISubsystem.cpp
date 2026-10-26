// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/SaveGISubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Subsystem/BossSetupGISubsystem.h"
#include "Subsystem/FlipSideSaveGame.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"
#include "Subsystem/ShopLevel/ShopCoinWSubsystem.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

const FString USaveGISubsystem::SaveSlotName = TEXT("FlipSideSave");

bool USaveGISubsystem::SaveCurrentGame()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance))
	{
		return false;
	}

	UFlipSideSaveGame* SaveData = Cast<UFlipSideSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UFlipSideSaveGame::StaticClass()));
	if (!IsValid(SaveData))
	{
		return false;
	}

	if (ULevelGISubsystem* LevelSubsystem = GameInstance->GetSubsystem<ULevelGISubsystem>())
	{
		SaveData->CurrentStageIndex = LevelSubsystem->GetBattleLevelIndex();
	}

	if (UBossSetupGISubsystem* BossSubsystem = GameInstance->GetSubsystem<UBossSetupGISubsystem>())
	{
		SaveData->StageBossAssignments = BossSubsystem->GetStageBossAssignments();
	}

	if (UMoneyGISubsystem* MoneySubsystem = GameInstance->GetSubsystem<UMoneyGISubsystem>())
	{
		SaveData->CurrentMoney = MoneySubsystem->GetCurrentMoney();
	}

	if (UUnlockGISubsystem* UnlockSubsystem = GameInstance->GetSubsystem<UUnlockGISubsystem>())
	{
		SaveData->UnlockedWeaponIDs = UnlockSubsystem->GetUnlockWeaponArray();
		SaveData->UnlockedCardIDs = UnlockSubsystem->GetUnlockCardArray();
	}

	UWorld* World = GameInstance->GetWorld();
	UShopCoinWSubsystem* CoinSubsystem = IsValid(World) ? World->GetSubsystem<UShopCoinWSubsystem>() : nullptr;
	UShopItemWSubsystem* ItemSubsystem = IsValid(World) ? World->GetSubsystem<UShopItemWSubsystem>() : nullptr;

	// 전투 월드에는 상점 서브시스템이 없으므로, 같은 스테이지의 상점 저장값을 유지합니다.
	if ((!IsValid(CoinSubsystem) || !IsValid(ItemSubsystem)) && DoesSavedGameExist())
	{
		if (UFlipSideSaveGame* PreviousSave = Cast<UFlipSideSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
			IsValid(PreviousSave) && PreviousSave->CurrentStageIndex == SaveData->CurrentStageIndex)
		{
			SaveData->ShopCoinSlots = PreviousSave->ShopCoinSlots;
			SaveData->ShopItemSlots = PreviousSave->ShopItemSlots;
		}
	}

	if (IsValid(CoinSubsystem))
	{
		const int32 SlotCount = CoinSubsystem->GetUnlockCoinSlotCount();
		SaveData->ShopCoinSlots.Reset(SlotCount);
		for (int32 SlotIndex = 0; SlotIndex < SlotCount; ++SlotIndex)
		{
			FCoinTypeStructure CoinData = CoinSubsystem->GetCoinSlotCoinType(SlotIndex);
			CoinData.SlotNum = SlotIndex;
			SaveData->ShopCoinSlots.Add(CoinData);
		}
	}

	if (IsValid(ItemSubsystem))
	{
		SaveData->ShopItemSlots = ItemSubsystem->GetPlayerItemArray();
	}

	return UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, SaveUserIndex);
}

bool USaveGISubsystem::LoadCurrentGame()
{
	bHasLoadedShopCoinSlots = false;
	bHasLoadedShopItemSlots = false;
	LoadedShopCoinSlots.Reset();
	LoadedShopItemSlots.Reset();
	if (!DoesSavedGameExist())
	{
		return false;
	}

	UFlipSideSaveGame* SaveData = Cast<UFlipSideSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(SaveData) || !IsValid(GameInstance))
	{
		return false;
	}

	if (UUnlockGISubsystem* UnlockSubsystem = GameInstance->GetSubsystem<UUnlockGISubsystem>())
	{
		UnlockSubsystem->ApplyUnlockedWeaponIDs(SaveData->UnlockedWeaponIDs);
		UnlockSubsystem->ApplyUnlockedCardIDs(SaveData->UnlockedCardIDs);
	}

	if (ULevelGISubsystem* LevelSubsystem = GameInstance->GetSubsystem<ULevelGISubsystem>())
	{
		LevelSubsystem->SetBattleLevelIndex(SaveData->CurrentStageIndex);
	}

	if (UBossSetupGISubsystem* BossSubsystem = GameInstance->GetSubsystem<UBossSetupGISubsystem>())
	{
		if (BossSubsystem->SetStageBossAssignments(SaveData->StageBossAssignments))
		{
			if (const int32* SavedBossID = SaveData->StageBossAssignments.Find(SaveData->CurrentStageIndex))
			{
				BossSubsystem->PrepareBossForSavedData(SaveData->CurrentStageIndex, *SavedBossID);
			}
		}
	}

	if (UMoneyGISubsystem* MoneySubsystem = GameInstance->GetSubsystem<UMoneyGISubsystem>())
	{
		MoneySubsystem->SetCurrentMoney(SaveData->CurrentMoney);
	}

	LoadedShopCoinSlots = SaveData->ShopCoinSlots;
	LoadedShopItemSlots = SaveData->ShopItemSlots;
	bHasLoadedShopCoinSlots = true;
	bHasLoadedShopItemSlots = true;
	return true;
}

bool USaveGISubsystem::ConsumeLoadedShopCoinSlots(TArray<FCoinTypeStructure>& OutSlots)
{
	if (!bHasLoadedShopCoinSlots) return false;
	OutSlots = MoveTemp(LoadedShopCoinSlots);
	bHasLoadedShopCoinSlots = false;
	return true;
}

bool USaveGISubsystem::ConsumeLoadedShopItemSlots(TArray<FSelectItem>& OutSlots)
{
	if (!bHasLoadedShopItemSlots) return false;
	OutSlots = MoveTemp(LoadedShopItemSlots);
	bHasLoadedShopItemSlots = false;
	return true;
}

bool USaveGISubsystem::ResetSavedGame()
{
	LoadedShopCoinSlots.Reset();
	LoadedShopItemSlots.Reset();
	bHasLoadedShopCoinSlots = false;
	bHasLoadedShopItemSlots = false;
	bool bDeleteSucceeded = true;
	if (DoesSavedGameExist())
	{
		bDeleteSucceeded = UGameplayStatics::DeleteGameInSlot(SaveSlotName, SaveUserIndex);
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UUnlockGISubsystem* UnlockSubsystem = GameInstance->GetSubsystem<UUnlockGISubsystem>())
		{
			UnlockSubsystem->ResetUnlockData();
		}
	}

	return bDeleteSucceeded;
}

bool USaveGISubsystem::DoesSavedGameExist() const
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex);
}

void USaveGISubsystem::SetReturnedToStartMenu(bool bInReturnedToStartMenu)
{
	bReturnedToStartMenu = bInReturnedToStartMenu;
}

bool USaveGISubsystem::HasReturnedToStartMenu() const
{
	return bReturnedToStartMenu;
}

