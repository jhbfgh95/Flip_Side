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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;
	class UDataManagerSubsystem* DataManagerSubsystem;

protected:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSecen;
	
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShowPanelMesh;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ReadyPanelMesh;

private:
	//타임라인들
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* CircularTimeline;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* LinearTimeline;

private:
	//설명 위젯
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ExplainWidget;

	class UW_WeaponDescription* ExplainWidgetClass;

public:
	UFUNCTION()
	void InitPannel(FCoinTypeStructure CoinInfo, EWeaponClass CoinClass);
	UFUNCTION()
	void GetExplainByID(int32 ID, bool IsFront);

protected:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FRotator ShowRotation;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CircularCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* LinearCurve;
	

private:
	//패널 직선움직임
	UFUNCTION()
	void MoveLinearPanel(float Value);
	//직선움직임이 끝났을 떄
	UFUNCTION()
	void FinishedMoveLinear();
	//원형 움직임
	UFUNCTION()
	void MoveCircularPanel(float Angle);
	//원형 움직임이 끝났을 때
	UFUNCTION()
	void FinishedMoveCirCular();

// 움직임 내부값들 
private:
	FVector ShowPanelLocation;
	FVector ReadyPanelLocation;
	FVector PanelArriveLocation;
	FVector PanelStartLocation;
	FVector ShowToReadyPanelGap;

	float Radius;
	float ShowPanelRadius;

private:
	int32 CurrentID;
	void SetDescriptionText(int32 ID);

public:
//설명 판넬이 직선 방향으로 얼마나 움직이게 할 것인가?
	UPROPERTY(EditAnywhere, Category = "AnimSetting")
	float PanelLinearMoveValue;
	

};
