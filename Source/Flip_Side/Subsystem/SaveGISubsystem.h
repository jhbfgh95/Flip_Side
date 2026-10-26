// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/ItemDataTypes.h"
#include "SaveGISubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API USaveGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 현재 GI Subsystem이 보관 중인 진행 데이터를 단일 저장 슬롯에 기록합니다.
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool SaveCurrentGame();

	// GI 데이터는 즉시, 상점 슬롯은 다음 상점 월드 초기화 시 복원합니다.
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool LoadCurrentGame();

	bool ConsumeLoadedShopCoinSlots(TArray<FCoinTypeStructure>& OutSlots);
	bool ConsumeLoadedShopItemSlots(TArray<FSelectItem>& OutSlots);

	// 단일 저장 슬롯 파일을 삭제해 저장된 진행 데이터를 초기화합니다.
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	bool ResetSavedGame();

	UFUNCTION(BlueprintPure, Category = "Save Game")
	bool DoesSavedGameExist() const;

	// 현재 실행 중 시작 화면으로 복귀했는지 기록합니다. 게임 종료 시 Subsystem과 함께 초기화됩니다.
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	void SetReturnedToStartMenu(bool bInReturnedToStartMenu);

	UFUNCTION(BlueprintPure, Category = "Save Game")
	bool HasReturnedToStartMenu() const;

private:
	UPROPERTY(Transient)
	TArray<FCoinTypeStructure> LoadedShopCoinSlots;

	UPROPERTY(Transient)
	TArray<FSelectItem> LoadedShopItemSlots;

	bool bHasLoadedShopCoinSlots = false;
	bool bHasLoadedShopItemSlots = false;

	static const FString SaveSlotName;
	static constexpr int32 SaveUserIndex = 0;

	bool bReturnedToStartMenu = false;
};
