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
	// 설명 메쉬 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* DescriptionMesh;

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

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PanelWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DescribeWidget;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* DescriptionTimeLine;
	
	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* DescriptionPanelCurve;

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
	class UShopWeaponDataWSubsystem* ShopWeaponDataSubsystem;
	class UDataManagerSubsystem* DataManagerSubsystem;

private:
	class UW_CoinManagePanelWidget* PanelWidgetClass;

	private:
	FVector LockPanelStartVector;
private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionPanelMoveDirection;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionPanelStartVec;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionPanelArriveVec;
	

private:
	UFUNCTION()
	void SetPanelWidget(int32 CoinSlotIndex, int32 CoinCount);

public:

	void InitPanel();
	void InitPanelAfterArrive();
	void InitPanelToStart();

	UFUNCTION()
	void MoveDescriptionPanel(float Value);

	UFUNCTION()
	void OpenDescriptionPanel();

	void ActiveDescriptionPanel(bool IsPanelShow);

	void InitPanelCoin();
};
