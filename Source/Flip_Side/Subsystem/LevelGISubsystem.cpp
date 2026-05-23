#include "LevelGISubsystem.h"
#include "BossSetupGISubsystem.h"
#include "CrossingLevelGISubsystem.h"
#include "MoneyGISubsystem.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    constexpr int32 TutorialPipeWeaponID = 1;
    constexpr int32 TutorialSniperWeaponID = 6;
    constexpr int32 TutorialAdrenalineWeaponID = 14;
    constexpr int32 TutorialRangeAmplifierWeaponID = 15;

    constexpr int32 TutorialPhaseChangeItemID = 4;
    constexpr int32 TutorialWallItemID = 5;
    constexpr int32 TutorialEverywhereItemID = 6;

    constexpr int32 TutorialOneForAllCardID = 5;
    constexpr int32 TutorialAllianceCardID = 6;

    void SetTutorialCoin(UCrossingLevelGISubsystem* CrossingLevelSubsystem, int32 SlotNum, int32 FrontWeaponID, int32 BackWeaponID, int32 CoinCount)
    {
        if (!CrossingLevelSubsystem)
        {
            return;
        }

        FCoinTypeStructure CoinData;
        CoinData.FrontWeaponID = FrontWeaponID;
        CoinData.BackWeaponID = BackWeaponID;
        CoinData.SameTypeCoinNum = CoinCount;
        CrossingLevelSubsystem->SetSlotCoin(SlotNum, CoinData);
    }

    void PrepareBattleTutorialLoadout(UCrossingLevelGISubsystem* CrossingLevelSubsystem)
    {
        if (!CrossingLevelSubsystem)
        {
            return;
        }

        CrossingLevelSubsystem->ResetRunData();
        CrossingLevelSubsystem->SetIsCoinEmpty(false);

        SetTutorialCoin(CrossingLevelSubsystem, 0, TutorialPipeWeaponID, TutorialSniperWeaponID, 20);
        SetTutorialCoin(CrossingLevelSubsystem, 1, TutorialAdrenalineWeaponID, TutorialRangeAmplifierWeaponID, 10);

        CrossingLevelSubsystem->SetBattleUseItemID(TutorialWallItemID, 0, 10);
        CrossingLevelSubsystem->SetBattleUseItemID(TutorialPhaseChangeItemID, 1, 10);
        CrossingLevelSubsystem->SetBattleUseItemID(TutorialEverywhereItemID, 2, 10);

        CrossingLevelSubsystem->SetBattleCardID(TutorialOneForAllCardID, 0);
        CrossingLevelSubsystem->SetBattleCardID(TutorialAllianceCardID, 1);
        CrossingLevelSubsystem->SetBattleCardID(-1, 2);
    }
}

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
        if (UCrossingLevelGISubsystem* CrossingLevelSubsystem = GetGameInstance()->GetSubsystem<UCrossingLevelGISubsystem>())
        {
            PrepareBattleTutorialLoadout(CrossingLevelSubsystem);
        }

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
        BattleLevelIndex = 1;

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
