// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleClickInterface.h"
#include "Base_OtherActor.generated.h"

UCLASS()
class FLIP_SIDE_API ABase_OtherActor : public AActor, public IBattleClickInterface
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Others | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* OthersMesh;

	UPROPERTY(VisibleAnywhere, Category = "Others | Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> OtherHPUI;

public:	
	ABase_OtherActor();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnClicked_Implementation() override;

};
