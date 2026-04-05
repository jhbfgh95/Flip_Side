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
	UFUNCTION(BlueprintCallable)
	void MoveBattleLevel();

	//상점 넘어가는 용도
	UFUNCTION(BlueprintCallable)
	void MoveShopLevel();

	//int값으로 0은 상점 튜토리얼, 1은 배틀 튜토리얼로 넘어감.
	//이거 이렇게 한 이유? 나중에 튜토리얼 추가되면 걍 이거로 넘어다니게
	UFUNCTION(BlueprintCallable)
	void MovingTutorialLevel(int32 tutorialflag);

	UFUNCTION(BlueprintCallable)
	void MoveStartLevel();

	//상점 -> 레벨 넘어가는 레벨 인덱스 넘겨줌. ShopWidget의 BattleLevelNames따라감
	int32 GetBattleLevelIndex();
};
