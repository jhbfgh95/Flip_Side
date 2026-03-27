// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopTutorialInterface.h"
#include "Shop_Tuto_MoveEventActor.generated.h"

UCLASS()
class FLIP_SIDE_API AShop_Tuto_MoveEventActor : public AActor, public IShopTutorialInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShop_Tuto_MoveEventActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class AGameMode_Shop* ShopGameMode;
	class UShopTutorialWSubsystem* TutorialSubsystem;
private:
	int CuurentEventCount = 0;

	void ExcuteEvent();

	virtual void ExecuteTutorialEvent_Implementation() override;
	
	//virtual void FinishTutorialEvent_Implementation() override;
	private:
	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow))
	int32 UnlockWeaponModeOrder;

	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow))
	int32 SetCoinManageModeOrder;

	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow))
	int32 SetUnlockCardModeOrder;

	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow))
	int32 SetCardSelectModeOrder;

	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow))
	int32 SetShopItemModeOrder;
	
	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow))
	int32 SetShopMainModeOrder;
};
