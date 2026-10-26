#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/ItemDataTypes.h"
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

	// 구매한 코인 슬롯 순서입니다. 무기 미장착·수량 0인 슬롯도 보존합니다.
	UPROPERTY(SaveGame)
	TArray<FCoinTypeStructure> ShopCoinSlots;

	// 배열 인덱스가 아이템 슬롯 번호입니다. 빈 슬롯도 그대로 저장합니다.
	UPROPERTY(SaveGame)
	TArray<FSelectItem> ShopItemSlots;
};
