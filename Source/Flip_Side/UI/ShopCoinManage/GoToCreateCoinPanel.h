// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "GoToCreateCoinPanel.generated.h"

UCLASS()
class FLIP_SIDE_API AGoToCreateCoinPanel : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoToCreateCoinPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	

private:
	// 루트 씬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class USceneComponent* RootScene;
	// 몸통 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* PanelMesh;
	//잠긴 판넬 표시 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* LockPanelMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* LockPanelTimeLine;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* ButtonTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ItneractBox;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "TimeLine")
	class UCurveFloat* LockPanelCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "TimeLine")
	class UCurveFloat* ButtonCurve;

private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
	class UCoinCreateWSubsystem* ShopCoinCreateSubsystem;
	class AGameMode_Shop* ShopGameMode;
	class AShopController_FlipSide* ShopController;
private:

	UFUNCTION()
	void InitLockPanel(bool IsChangetoBottom);

	UFUNCTION(BlueprintCallAble)
	void OpenlockPanel();
	
	UFUNCTION()
	void MoveLockPanel(float Value);

	UFUNCTION()
	void MoveButton(float Value);

	
	UFUNCTION()
	void FinishedMoveButton();

	UFUNCTION()
	void FinishMoveLockPanel();

	void ChangeCreateCoinMode();

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector PressTargetVector;

	FVector PressArriveVector;
	FVector PressStartVector;


private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector TargetVector;

	FVector ArriveVector;
	FVector StartVector;

	bool IsLockPanelOpen = false;

public:
	virtual void InteractLeftClick_Implementation() override;
};
