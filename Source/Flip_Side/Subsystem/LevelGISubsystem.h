// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelGISubsystem.generated.h"

UCLASS()
class FLIP_SIDE_API ULevelGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	int32 BattleLevelIndex = 0;

public:
	void MoveBattleLevel(const FString& LevelName);

	void MoveLevel(const FString& LevelName);

	//상점 -> 레벨 넘어가는 레벨 인덱스 넘겨줌. ShopWidget의 BattleLevelNames따라감
	int32 GetBattleLevelIndex();
};
