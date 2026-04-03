// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/CoinDataTypes.h"
#include "ShopCoinManagePanel.generated.h"

UCLASS()
class FLIP_SIDE_API AShopCoinManagePanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopCoinManagePanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// 루트 씬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class USceneComponent* RootScene;
	// 몸통 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* PanelMesh;
	// 기어 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UChildActorComponent* GearActor;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UChildActorComponent* IncreaseCountButton;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UChildActorComponent* DecreaseCountButton;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UChildActorComponent* CoinActor;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UChildActorComponent* LockPanel;

private:
	class AShopCoinSlotCountButton* IncreaseCountButtonClass;
	class AShopCoinSlotCountButton* DecreaseCountButtonClass;
	class ACoinSlotLockPanel* LockPanelClass;
	class AShopCoinManageCoin* CoinActorClass;
	class ACoinSlotWeaponDescripPanel* DescripClass;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PanelWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DescribeWidget;

private:
	class UW_CoinManagePanelWidget* PanelWidgetClass;
	
	class UW_WeaponDescription* WeaponDesWidgetClass;


private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
	class UDataManagerSubsystem* DataManagerSubsystem;


	private:
	FVector LockPanelStartVector;
	private:
	FVector DescripanelStartVector;
	


private:

private:
	UFUNCTION()
	void SetPanelWidget(int32 CoinSlotIndex, int32 CoinCount);


public:
	UFUNCTION()
	void InitPanel();


	void InitPanelCoin();
};
