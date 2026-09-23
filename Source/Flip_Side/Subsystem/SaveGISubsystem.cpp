// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/SaveGISubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Subsystem/BossSetupGISubsystem.h"
#include "Subsystem/FlipSideSaveGame.h"
#include "Subsystem/LevelGISubsystem.h"
#include "Subsystem/MoneyGISubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"

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

	return UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, SaveUserIndex);
}

bool USaveGISubsystem::LoadCurrentGame()
{
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

	return true;
}

bool USaveGISubsystem::ResetSavedGame()
{
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

