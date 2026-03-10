// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ItemLogicLibrary.h"

void UItemLogicLibrary::Test_Item(UItem_Action* ItemContext)
{
    UE_LOG(LogTemp, Warning, TEXT("Test Item"));
}

void UItemLogicLibrary::MeltShieldPotion_Logic(UItem_Action* ItemContext)
{
    UE_LOG(LogTemp, Warning, TEXT("MeltShield"));
}

void UItemLogicLibrary::BloodPotion_Logic(UItem_Action* ItemContext)
{
    UE_LOG(LogTemp, Warning, TEXT("Blood"));
}

void UItemLogicLibrary::CleanserPotion_Logic(UItem_Action* ItemContext)
{
    UE_LOG(LogTemp, Warning, TEXT("Cleanser"));
}

void UItemLogicLibrary::PhaseChangePotion_Logic(UItem_Action* ItemContext)
{
    UE_LOG(LogTemp, Warning, TEXT("PhaseChange"));
}

void UItemLogicLibrary::EverwherePotion_Logic(UItem_Action* ItemContext)
{
    UE_LOG(LogTemp, Warning, TEXT("Everywhere"));
}