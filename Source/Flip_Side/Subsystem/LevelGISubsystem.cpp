#include "LevelGISubsystem.h"
#include "BossSetupGISubsystem.h"
#include "MoneyGISubsystem.h"
#include "CrossingLevelGISubsystem.h"
#include "FlipSideDevloperSettings.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void ULevelGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

#if WITH_EDITOR
    const UWorld* World = GetWorld();
    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (!IsValid(World) || World->WorldType != EWorldType::PIE ||
        !World->GetName().Contains(TEXT("L_Stage_BattleTutorial")) ||
        !IsValid(Settings) || !Settings->bStartBattleTutorialInPIE)
        return;

    UBossSetupGISubsystem* BossSetup = Collection.InitializeDependency<UBossSetupGISubsystem>();
    if (!IsValid(BossSetup) || !BossSetup->PrepareBossForID(1))
    {
        UE_LOG(LogTemp, Warning, TEXT("[Tutorial] PIE direct start failed to prepare BossID 1."));
        return;
    }

    BattleLevelIndex = 0;
    bRunBattleTutorial = true;
    UE_LOG(LogTemp, Log, TEXT("[Tutorial] PIE direct start enabled: %s (BossID 1)."), *World->GetName());
#endif
}

void ULevelGISubsystem::MoveBattleLevel()
{
    bRunBattleTutorial = false;
    bRunTutorialBossBattle = false;
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
    bRunTutorialBossBattle = false;
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
    bRunTutorialBossBattle = false;
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_ShopLevel")));
}

void ULevelGISubsystem::MovingTutorialLevel(int32 tutorialflag)
{
    if (tutorialflag < 0 || tutorialflag > 2 || !IsValid(GetGameInstance())) return;
    UGameInstance* GI = GetGameInstance();
    UBossSetupGISubsystem* BossSetup = GI->GetSubsystem<UBossSetupGISubsystem>();
    if (!IsValid(BossSetup) || !BossSetup->PrepareBossForID(1)) return;
    BattleLevelIndex = 0;
    bRunBattleTutorial = tutorialflag == 0;
    bRunTutorialBossBattle = tutorialflag == 1;
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

bool ULevelGISubsystem::IsTutorialBossBattleActive() const
{
    return bRunTutorialBossBattle && IsValid(GetWorld()) &&
        GetWorld()->GetName().Contains(TEXT("L_Stage_BattleTutorial"));
}

int32 ULevelGISubsystem::GetBattleLevelIndex()
{
    return BattleLevelIndex;
}

void ULevelGISubsystem::SetBattleLevelIndex(int32 InBattleLevelIndex)
{
    bRunTutorialBossBattle = false;
    BattleLevelIndex = FMath::Max(0, InBattleLevelIndex);
}

void ULevelGISubsystem::MoveStartLevel()
{
    bRunBattleTutorial = false;
    bRunTutorialBossBattle = false;
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
