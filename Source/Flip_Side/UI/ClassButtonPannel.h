// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClassButtonPannel.generated.h"

UCLASS()
class FLIP_SIDE_API AClassButtonPannel : public AActor
{
	GENERATED_BODY()
	


public:	
	// Sets default values for this actor's properties
	AClassButtonPannel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	class UCoinCreateWSubsystem* coinCreateWSubSystem;
private:
	
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PannelMesh;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* Timeline;

private:
	bool bIsPannelOpen;
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector StartLocation;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FVector EndLocation;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* PannelMoveCurve;

	UFUNCTION()
	void MovePannel(float Value);

	UFUNCTION()
	void FinishMovePannel();
	UFUNCTION(BlueprintCallAble)
	void WorkPannel();
	
	void OpenPannel();

	void ClosePannel();

};
