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
	
	// 몸통 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* GearMesh;

	// 몸통 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* DescribeMesh;

	//잠긴 판넬 표시 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* LockPanelMesh;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* PanelTimeLine;

	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* PaneDownlTimeLine;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* UnlockTimeLine;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PanelWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DescribeWidget;

private:

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* PanelRotateCurve;
	
	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* UnlockPanelCurve;

	//판넬움직임
	UFUNCTION()
	void RotatePanel(float Value);
	
	UFUNCTION()
	void RunPanel(bool IsInCrease);


	UFUNCTION()
	void MovePanelUp(float Value);

	UFUNCTION()
	void MovePanelDown(float Value);

	UFUNCTION()
	void FinishedPanelMove();

	

	UFUNCTION()
	void LockPanelMove(float Value);
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector TargetVector;

	FVector StartVector;
	FVector ArriveVector;

private:
	
	//판넬설정
	void InitPanel();

	//타임라인 추가
	// 잠금 판넬
	// 동전 클래스?
	// 코인 능력 설명 클래스
	// 판넬 내부 UI

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;

private:

	FVector PanelStartVector;
	FVector PanelBottomArriveVector;
	FVector PanelTopArriveVector;
	FVector PanelMoveTopDirect;
public:
	UFUNCTION(BlueprintCallAble)
	void ChangeSlot();
	UFUNCTION(BlueprintCallAble)
	void UnlockPanel();
};
