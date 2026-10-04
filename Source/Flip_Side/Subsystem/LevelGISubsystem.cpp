#include "LevelGISubsystem.h"
#include "BossSetupGISubsystem.h"
#include "MoneyGISubsystem.h"
#include "CrossingLevelGISubsystem.h"
#include "Kismet/GameplayStatics.h"

void ULevelGISubsystem::MoveBattleLevel()
{
    bRunBattleTutorial = false;
    UGameInstance* GI = Cast<UGameInstance>(GetOuter());
    if (GI)
    {
        if (UCrossingLevelGISubsystem* Crossing = GI->GetSubsystem<UCrossingLevelGISubsystem>())
            if (UMoneyGISubsystem* Money = GI->GetSubsystem<UMoneyGISubsystem>())
                Crossing->SetBattleEntryGold(Money->GetCurrentMoney());
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
    bRunBattleTutorial = false;
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

void ULevelGISubsystem::MoveLoadedShopLevel()
{
    bRunBattleTutorial = false;
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_ShopLevel")));
}

void ULevelGISubsystem::MovingTutorialLevel(int32 tutorialflag)
{
    if (tutorialflag < 0 || tutorialflag > 2 || !IsValid(GetGameInstance())) return;
    BattleLevelIndex = 0;
    bRunBattleTutorial = tutorialflag == 0;
    UGameInstance* GI = GetGameInstance();
    if (UBossSetupGISubsystem* BossSetup = GI->GetSubsystem<UBossSetupGISubsystem>(); IsValid(BossSetup))
    {
        if (!BossSetup->PrepareBossForID(1)) return;
    }
    if (tutorialflag != 2)
        if (UCrossingLevelGISubsystem* Crossing = GI->GetSubsystem<UCrossingLevelGISubsystem>(); IsValid(Crossing))
            if (UMoneyGISubsystem* Money = GI->GetSubsystem<UMoneyGISubsystem>(); IsValid(Money))
                Crossing->SetBattleEntryGold(Money->GetCurrentMoney());
    UGameplayStatics::OpenLevel(GetWorld(), tutorialflag == 2
        ? FName(TEXT("L_Tutorial_TutoShop_Level")) : FName(TEXT("L_Stage_BattleTutorial")));
}

bool ULevelGISubsystem::IsBattleTutorialActive() const
{
    return bRunBattleTutorial && IsValid(GetWorld()) &&
        GetWorld()->GetName().Contains(TEXT("L_Stage_BattleTutorial"));
}

int32 ULevelGISubsystem::GetBattleLevelIndex()
{
    return BattleLevelIndex;
}

void ULevelGISubsystem::SetBattleLevelIndex(int32 InBattleLevelIndex)
{
    BattleLevelIndex = FMath::Max(0, InBattleLevelIndex);
}

void ULevelGISubsystem::MoveStartLevel()
{
    bRunBattleTutorial = false;
    if (UCrossingLevelGISubsystem* Crossing = GetGameInstance()->GetSubsystem<UCrossingLevelGISubsystem>())
        Crossing->ResetBattleEntryGold();
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
