// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/Shop_Tuto_MoveEventActor.h"
#include "Player/GameMode_Shop.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
// Sets default values
AShop_Tuto_MoveEventActor::AShop_Tuto_MoveEventActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AShop_Tuto_MoveEventActor::BeginPlay()
{
	Super::BeginPlay();
	ShopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
	TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();

	TutorialSubsystem->SetTutorialEvent(UnlockWeaponModeOrder,this);
	TutorialSubsystem->SetTutorialEvent(SetCoinManageModeOrder,this);
	TutorialSubsystem->SetTutorialEvent(SetUnlockCardModeOrder,this);
	TutorialSubsystem->SetTutorialEvent(SetCardSelectModeOrder,this);
	TutorialSubsystem->SetTutorialEvent(SetShopItemModeOrder,this);
	TutorialSubsystem->SetTutorialEvent(SetShopMainModeOrder,this);
	TutorialSubsystem->SetTutorialEvent(SetBossStateModeOrder,this);

}

	
void AShop_Tuto_MoveEventActor::ExecuteTutorialEvent_Implementation()
{
	switch (CuurentEventCount)
	{
		case 0:
			ShopGameMode->SetCheckBossMode();
			
			break;
		case 1:
			ShopGameMode->SetUnlockWeaponMode();
			break;
		case 2:
			ShopGameMode->SetCoinManageMode();
			break;
		case 3:
			ShopGameMode->SetUnlockCardMode();
			break;
		case 4:
			ShopGameMode->SetCardSelectMode();
			break;
		case 5:
			ShopGameMode->SetShopItemMode();
			break;
		case 6:
			ShopGameMode->SetShopMainMode();
			break;
		case 7:
			ShopGameMode->SetShopMainMode();
			break;
		default:
			break;
	}
	CuurentEventCount++;
}