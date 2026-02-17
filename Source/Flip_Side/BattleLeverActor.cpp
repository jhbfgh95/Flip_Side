#include "BattleLeverActor.h"
#include "Subsystem/CoinManagementWSubsystem.h" 
#include "Subsystem/BattleManagerWSubsystem.h" 

void ABattleLeverActor::ExecuteLeverLogic()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // 코인락
    if (UCoinManagementWSubsystem* CoinSubsystem = World->GetSubsystem<UCoinManagementWSubsystem>())
    {
        CoinSubsystem->LockCoinReady();
    }

    // 턴 전환
    if (UBattleManagerWSubsystem* BattleSubsystem = World->GetSubsystem<UBattleManagerWSubsystem>())
    {
        BattleSubsystem->StartBattleFromLever();
    }
}