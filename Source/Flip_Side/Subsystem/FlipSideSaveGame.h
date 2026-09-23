#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FlipSideSaveGame.generated.h"

UCLASS()
class FLIP_SIDE_API UFlipSideSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	int32 CurrentStageIndex = 0;

	// 이번 플레이에서 랜덤으로 결정된 스테이지별 보스 등장 순서입니다.
	UPROPERTY(SaveGame)
	TMap<int32, int32> StageBossAssignments;

	UPROPERTY(SaveGame)
	int32 CurrentMoney = 0;

	UPROPERTY(SaveGame)
	TArray<int32> UnlockedWeaponIDs;

	UPROPERTY(SaveGame)
	TArray<int32> UnlockedCardIDs;
};
