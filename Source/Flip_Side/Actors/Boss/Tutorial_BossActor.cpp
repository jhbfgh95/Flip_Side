// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial_BossActor.h"

ATutorial_BossActor::ATutorial_BossActor()
{
    MaxShield = MAX_TUTO_BOSS_SHIELD;
    CurrentShield = MAX_TUTO_BOSS_SHIELD;
}

void ATutorial_BossActor::BeginPlay()
{
	Super::BeginPlay();
}
