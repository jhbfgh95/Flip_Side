// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelGISubsystem.generated.h"

UCLASS()
class FLIP_SIDE_API ULevelGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	//이거 나중에 0으로 바꿔야 함. 
	//왜냐면 0번인 튜토리얼 스테이지는 걍 전용함수로 건너다니기 때문
	int32 BattleLevelIndex = 0;

public:
	//배틀레벨 넘어가는 용도
	void MoveBattleLevel(const FString& LevelName);

	//상점 넘어가는 용도
	void MoveLevel(const FString& LevelName);

	//int값으로 0은 상점 튜토리얼, 1은 배틀 튜토리얼로 넘어감.
	//이거 이렇게 한 이유? 나중에 튜토리얼 추가되면 걍 이거로 넘어다니게
	void MovingTutorialLevel(int32 tutorialflag);

	//상점 -> 레벨 넘어가는 레벨 인덱스 넘겨줌. ShopWidget의 BattleLevelNames따라감
	int32 GetBattleLevelIndex();
};
