// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataTypes/FlipSide_Enum.h"
#include "UnlockGISubsystem.generated.h"

/**
 * 
 */


//선택된 코인이 변경되었을 때 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnlockWeapon, int32, UnlockWeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnlockCard, int32, CardID);

UCLASS()
class FLIP_SIDE_API UUnlockGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OnLevelLoad(UWorld* LoadedWorld);
private:
	TArray<int32> UnlockWeaponArray;

	TArray<int32> CardUnlockArray;

public:
	FUnlockWeapon OnWeaponUnlock; 
	FUnlockCard OnUnlockCard;

public:

	void UnlockWeapon(int32 ID);

	void UnlockCard(int32 ID);

	int32 GetUnlockWeaponArrrayNum();

	int32 GetUnlockCardArrayNum();

	bool IsWeaponUnlockByID(int32 ID);

	bool IsCardUnlockByID(int32 ID);

	void ResetUnlockData();

	// SaveGame에서 읽은 해금 무기 ID 목록을 현재 게임 상태에 적용합니다.
	void ApplyUnlockedWeaponIDs(const TArray<int32>& InUnlockedWeaponIDs);

	// SaveGame에서 읽은 해금 카드 ID 목록을 현재 게임 상태에 적용합니다.
	void ApplyUnlockedCardIDs(const TArray<int32>& InUnlockedCardIDs);

	const TArray<int32>& GetUnlockWeaponArray();
	const TArray<int32>& GetUnlockCardArray();
};
