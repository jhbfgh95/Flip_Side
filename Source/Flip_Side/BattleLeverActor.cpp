#include "BattleLeverActor.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h" 
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h" 
#include "FlipSide_Enum.h" // ETurnState 정의가 포함된 헤더를 반드시 포함하세요.

void ABattleLeverActor::ExecuteLeverLogic()
{
    UWorld* World = GetWorld();
    if (!World) return;

    auto* BattleSub = World->GetSubsystem<UBattleManagerWSubsystem>();
    auto* CoinSub = World->GetSubsystem<UCoinManagementWSubsystem>();
    
    if (!BattleSub || !CoinSub) return;

    ETurnState CurrentState = BattleSub->GetCurrentTurn();

    // 서랍에 코인 세팅 후 레버 당김
    if (CurrentState == ETurnState::CoinReadyTurn)
    {
        for(auto* Coin : CoinSub->GetReadyCoins())
        {
            CoinSub->LockCoinReady(Coin);
        }
        BattleSub->StartBattleFromLever(); // CoinReadyTurn -> CoinSelectTurn
    }

    // 원하는 행동 다 하고 레버 당김
    else if (CurrentState == ETurnState::CoinSelectTurn)
    {
        BattleSub->StartBattleFromLever(); // CoinSelecTurn -> BehaviorTurn
    }
}