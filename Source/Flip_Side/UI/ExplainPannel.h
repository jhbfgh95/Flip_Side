// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplainPannel.generated.h"

UCLASS()
class FLIP_SIDE_API AExplainPannel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplainPannel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



protected:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PannelMesh;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* Timeline;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ExplainWidget;

	class UW_ExplainWidget* ExplainWidgetClass;

protected:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FRotator ShowRotation;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* PannelRotationCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* PannelMoveCurve;

	//UFUNCTION()
	//void MovePannel(float Value);

	UFUNCTION()
	void FinishRotationPannel();

	UFUNCTION(BlueprintCallAble)
	void WorkPannel();
	UFUNCTION()
	void RotatePannel(float Value);

	UFUNCTION()
	virtual void GetExplainByID(int32 ID);
	
	void UpdateExplanationText(FString Explain);


	//void ClosePannel();

};
