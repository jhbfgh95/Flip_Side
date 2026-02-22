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
	class UStaticMeshComponent* DescritionMesh;

	//잠긴 판넬 표시 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* LockPanelMesh;

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

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* PanelRotateCurve;
	
	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* UnlockPanelCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* GearCurve;
private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector LockPanelStartVec;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector LockPanelArriveVec;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector LockPanelMoveDirection;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionPanelStartVec;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionPanelArriveVec;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float GearRotateAngle;


public:

	void InitPanel();

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

};
