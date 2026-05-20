#include "LevelGISubsystem.h"
#include "BossSetupGISubsystem.h"
#include "CrossingLevelGISubsystem.h"
#include "MoneyGISubsystem.h"
#include "Kismet/GameplayStatics.h"

void ULevelGISubsystem::MoveBattleLevel()
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
        UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_Tutorial_TutoShop_Level")));
    }
    else if(tutorialflag == 1)
    {
        UGameInstance* GI = Cast<UGameInstance>(GetOuter());
        if (GI)
        {
            UBossSetupGISubsystem* BossSetupGI = GI->GetSubsystem<UBossSetupGISubsystem>();
            if (BossSetupGI)
            {
                BossSetupGI->PrepareBossForID(1);
            }
        }
        UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_Stage_Battle_Tutorial")));
    }
    else if(tutorialflag == 2)
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
}

int32 ULevelGISubsystem::GetBattleLevelIndex()
{
    return BattleLevelIndex;
}

void ULevelGISubsystem::MoveStartLevel()
{
    if (UCrossingLevelGISubsystem* CrossingLevel = GetGameInstance()->GetSubsystem<UCrossingLevelGISubsystem>())
    {
        CrossingLevel->ResetRunData();
    }
    if (UBossSetupGISubsystem* BossSetup = GetGameInstance()->GetSubsystem<UBossSetupGISubsystem>())
    {
        BossSetup->ResetBossStageAssignments();
    }
    if(UMoneyGISubsystem* MM = GetGameInstance()->GetSubsystem<UMoneyGISubsystem>())
    {
        MM->InitMoney();
    }

    BattleLevelIndex = 0;
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_GameStart")));
}
