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
	//배틀레벨 넘어가는 용도
	void MoveBattleLevel(const FString& LevelName);

	//상점 넘어가는 용도
	void MoveLevel(const FString& LevelName);

	//상점 -> 레벨 넘어가는 레벨 인덱스 넘겨줌. ShopWidget의 BattleLevelNames따라감
	int32 GetBattleLevelIndex();
};
