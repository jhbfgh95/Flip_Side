// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CreateCoinDescriptionPanel.generated.h"

UCLASS()
class FLIP_SIDE_API ACreateCoinDescriptionPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACreateCoinDescriptionPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;
	UFUNCTION()
	void InitPannel(FCoinTypeStructure CoinInfo, EWeaponClass CoinClass);

	UFUNCTION()
	void GetExplainByID(int32 ID);
protected:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSecen;
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShowPanelMesh;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ReadyPanelMesh;

	//타임라인들
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* CircularTimeline;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* LinearTimeline;

	/*
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ExplainWidget;

	class UW_ExplainWidget* ExplainWidgetClass;
	*/
protected:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FRotator ShowRotation;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CircularCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* LinearCurve;
	

public:
	UFUNCTION()
	void MoveLinearPanel(float Value);

	UFUNCTION()
	void FinishedMoveLinear();

	UFUNCTION()
	void MoveCircularPanel(float Angle);

	UFUNCTION()
	void FinishedMoveCirCular();

public:

	FVector ShowPanelLocation;
	FVector ReadyPanelLocation;

	FVector ShowToReadyPanelGap;

	FVector PanelArriveLocation;
	FVector PanelStartLocation;

	float ReadyPanelStartAngle; 

	float ReadyPanelArriveAngle;

	float Radius;

	float ShowPanelRadius;

	UPROPERTY(EditAnywhere, Category = "AnimSetting")
	float PanelLinearMoveValue;
	

};
