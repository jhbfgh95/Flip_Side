// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopItemSellPanel.generated.h"

UCLASS()
class FLIP_SIDE_API AShopItemSellPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopItemSellPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class USceneComponent* PanelAnchorComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UStaticMeshComponent* ItemSellPanel;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UTimelineComponent* PanelMoveTimeline;


private:
	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	class UCurveFloat* PanelMoveCurve;

	UPROPERTY(EditAnywhere, meta= (AllowPrivateAccess = "true"))
	FRotator TargetRotator;

	FRotator StartRotator;
	FRotator ArriveRotator;

public:
	UFUNCTION(BlueprintCallable)
	void OpenPanel();

	void ClosePanel();

	UFUNCTION()
	void PanelMove(float Value);

};
