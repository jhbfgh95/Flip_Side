// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_UnlockWeaponLever.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


ATuto_UnlockWeaponLever::ATuto_UnlockWeaponLever()
{

}
void ATuto_UnlockWeaponLever::BeginPlay()
{
    Super::BeginPlay();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
}

void ATuto_UnlockWeaponLever::ExecuteLeverLogic()
{
    Super::ExecuteLeverLogic();
    
    if(IsExecuteNextOrder)
        TutorialSubsytem->ExecuteEvents(NextOrder);
}