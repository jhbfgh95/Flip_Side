#include "LevelGISubsystem.h"
#include "Kismet/GameplayStatics.h"

void ULevelGISubsystem::MoveBattleLevel(const FString& LevelName)
{
    UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
    BattleLevelIndex++;
}

void ULevelGISubsystem::MoveLevel(const FString& LevelName)
{
    UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

void ULevelGISubsystem::MovingTutorialLevel(int32 tutorialflag)
{
    if(tutorialflag == 0)
    {
        //UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Tutorial_Shop_Level")));
    }
    else if(tutorialflag == 1)
    {
        //UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Tutorial_Battle_Level")));
    }
}

int32 ULevelGISubsystem::GetBattleLevelIndex()
{
    return BattleLevelIndex;
}