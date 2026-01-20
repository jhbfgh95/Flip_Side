// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GameMode_Shop.h"


void AGameMode_Shop::SetCoinCreateMode()
{
    OnCoinCreateMode.Broadcast();
}


void AGameMode_Shop::SetMainMode()
{
    OnMainMode.Broadcast();
}