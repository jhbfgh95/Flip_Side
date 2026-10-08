// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelGISubsystem.generated.h"

/*
	게임의 레벨 상태들을 관리해주는 GISubsystem이다.
*/

UCLASS()
class FLIP_SIDE_API ULevelGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	int32 BattleLevelIndex = 0;
	bool bRunTutorialBossBattle = false;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	//배틀레벨 넘어가는 용도
	UFUNCTION(BlueprintCallable)
	void MoveBattleLevel();

	//상점 넘어가는 용도
	UFUNCTION(BlueprintCallable)
	void MoveShopLevel();

	//저장된 진행도와 보스 배정을 유지한 채 상점으로 이동합니다.
	UFUNCTION(BlueprintCallable)
	void MoveLoadedShopLevel();

	// 0: 전투 설명 시작, 1: 상점 이후 실제 튜토리얼 보스전, 2: 상점 튜토리얼.
	UFUNCTION(BlueprintCallable)
	void MovingTutorialLevel(int32 tutorialflag);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tutorial")
	bool bRunBattleTutorial = false;

	bool IsBattleTutorialActive() const;
	bool IsTutorialBossBattleActive() const;

	//처음으로 돌아가면 무조건 초기화
	UFUNCTION(BlueprintCallable)
	void MoveStartLevel();

	//상점 -> 레벨 넘어가는 레벨 인덱스 넘겨줌. ShopWidget의 BattleLevelNames따라감
	int32 GetBattleLevelIndex();

	//저장 데이터를 불러올 때 현재 진행 스테이지를 복원합니다.
	void SetBattleLevelIndex(int32 InBattleLevelIndex);
};
