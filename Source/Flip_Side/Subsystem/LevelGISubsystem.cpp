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

int32 ULevelGISubsystem::GetBattleLevelIndex()
{
    return BattleLevelIndex;
}