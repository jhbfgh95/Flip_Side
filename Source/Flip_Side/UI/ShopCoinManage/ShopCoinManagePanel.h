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
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 루트 씬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class USceneComponent* RootScene;
	// 몸통 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* PanelMesh;
	
	// 기어 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* GearMesh;

	// 설명 메쉬 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* DescriptionMesh;

	//잠긴 판넬 표시 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* LockPanelMesh;

	//코인 표시 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* PanelCoinMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* IncreaseCountMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* DecreaseCountMesh;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UBoxComponent* UnlockPanelBox;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UBoxComponent* IncreaseCountBox;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UBoxComponent* DecreaseCountBox;


private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PanelWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DescribeWidget;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* DescriptionTimeLine;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* UnlockTimeLine;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* GearTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* PanelCoinTimeline;
	
	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* UnlockPanelCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* DescriptionPanelCurve;
	
	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* GearCurve;
private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
	class UShopWeaponDataWSubsystem* ShopWeaponDataSubsystem;
	class UDataManagerSubsystem* DataManagerSubsystem;

private:
	class UW_CoinManagePanelWidget* PanelWidgetClass;

private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector LockPanelStartVec;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector LockPanelArriveVec;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector LockPanelMoveDirection;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionPanelMoveDirection;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionPanelStartVec;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionPanelArriveVec;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float GearRotateAngle;

private:
	//코인 관련
	FRotator StartGearRotator;
	FRotator ArriveGearRotator;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FRotator GearRotateDirection;

private:
	UFUNCTION()
	void SetPanelWidget(int32 CoinSlotIndex, int32 CoinCount);


public:

	void InitPanel();
	void InitPanelAfterArrive();
	void InitPanelToStart();

	UFUNCTION(BlueprintCallAble)
	void UnlockPanel();

	UFUNCTION()
	void LockPanelMove(float Value);

	UFUNCTION()
	void MoveDescriptionPanel(float Value);

	UFUNCTION()
	void RotateGear(float Value);
	UFUNCTION()
	void ActiveGear(bool IsPanelMoveToBottom);

	UFUNCTION()
	void OpenDescriptionPanel();

	void ActiveDescriptionPanel(bool IsPanelShow);

	void InitPanelCoin();
};
