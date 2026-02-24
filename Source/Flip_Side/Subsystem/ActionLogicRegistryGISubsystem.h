#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ActionLogicRegistryGISubsystem.generated.h"

using FActionLogic = TFunction<void(class UAction_Base* Context)>;

UCLASS()
class FLIP_SIDE_API UActionLogicRegistryGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
/*
	// 각 카테고리별 로직 등록 함수
	void RegisterCoinLogic(FName ID, FActionLogic Logic) { CoinMap.Add(ID, Logic); }
	void RegisterBossLogic(FName ID, FActionLogic Logic) { BossMap.Add(ID, Logic); }
	void RegisterItemLogic(FName ID, FActionLogic Logic) { ItemMap.Add(ID, Logic); }

	// 카테고리에 맞는 명부에서 로직을 찾아 반환
	FActionLogic GetCoinLogic(FName ID) { return CoinMap.FindRef(ID); }
	FActionLogic GetBossLogic(FName ID) { return BossMap.FindRef(ID); }
	FActionLogic GetItemLogic(FName ID) { return ItemMap.FindRef(ID); }

private:
	// 명부를 나누어 관리하므로 ID가 겹쳐도 상관없음
	TMap<FName, FActionLogic> CoinMap;
	TMap<FName, FActionLogic> BossMap;
	TMap<FName, FActionLogic> ItemMap;
*/
	
};
