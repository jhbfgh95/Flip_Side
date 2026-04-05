#include "LevelGISubsystem.h"
#include "BossSetupGISubsystem.h"
#include "Kismet/GameplayStatics.h"

void ULevelGISubsystem::MoveBattleLevel()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_StageOne")));
}

void ULevelGISubsystem::MoveShopLevel()
{
    BattleLevelIndex++;
    UGameInstance* GI = Cast<UGameInstance>(GetOuter());
    if (GI)
    {
        UBossSetupGISubsystem* BossSetupGI = GI->GetSubsystem<UBossSetupGISubsystem>();
        if (BossSetupGI)
        {
            BossSetupGI->PrepareBossForStage(BattleLevelIndex);
        }
    }
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_ShopLevel")));
}

void ULevelGISubsystem::MovingTutorialLevel(int32 tutorialflag)
{
    //상점으로 넘어가기 때문에 prepare해둔다.
    BattleLevelIndex = 0;
    if(tutorialflag == 0)
    {
        UGameInstance* GI = Cast<UGameInstance>(GetOuter());
        if (GI)
        {
            UBossSetupGISubsystem* BossSetupGI = GI->GetSubsystem<UBossSetupGISubsystem>();
            if (BossSetupGI)
            {
                BossSetupGI->PrepareBossForStage(BattleLevelIndex);
            }
        }
        UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_Tutorial_Shop_Level")));
    }
    else if(tutorialflag == 1)
    {
        UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_Stage_Battle_Tutorial")));
    }
}

int32 ULevelGISubsystem::GetBattleLevelIndex()
{
    return BattleLevelIndex;
}

void ULevelGISubsystem::MoveStartLevel()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_GameStart")));
}