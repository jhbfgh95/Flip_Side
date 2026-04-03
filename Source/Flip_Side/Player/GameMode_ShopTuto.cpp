// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GameMode_ShopTuto.h"
#include "Subsystem/LevelGISubsystem.h"

void AGameMode_ShopTuto::ChangeBattleLevel()
{
    ULevelGISubsystem* LevelSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULevelGISubsystem>();
    LevelSubsystem->MovingTutorialLevel(1);
}