// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Tuto_ShopModeChanger.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Player/GameMode_Shop.h"
#include "Player/ShopController_FlipSide.h"
// Sets default values
ATuto_ShopModeChanger::ATuto_ShopModeChanger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATuto_ShopModeChanger::BeginPlay()
{
	Super::BeginPlay();
	TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
	TutoGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
	ShopController = Cast<AShopController_FlipSide>(GetWorld()->GetFirstPlayerController());
	if(TutoSubsystem)
	{
		for(int i =0; i< Order.Num(); i++)
			TutoSubsystem->SetTutorialEvent(Order[i],this);
	}
	
}

void ATuto_ShopModeChanger::ExecuteTutorialEvent_Implementation()
{
	switch (EventCount)
	{
	case 0:
		TutoGameMode->SetCheckBossMode();
		break;
	
	case 1:
		TutoGameMode->SetUnlockWeaponMode();
		break;
	
	case 2:
		TutoGameMode->SetCoinManageMode();
		break;
	
	case 3:
		TutoGameMode->SetShopItemMode();
		break;
	
	case 4:
		TutoGameMode->SetCardSelectMode();
		break;
		
	case 5:
		TutoGameMode->SetShopMainMode();
		break;
		
	default:
		break;
	}

	EventCount++;
}

void ATuto_ShopModeChanger::FinishTutorialEvent_Implementation()
{

}

