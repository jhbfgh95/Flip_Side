// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopTutorialInterface.h"
#include "Tuto_Arrow.generated.h"

UCLASS()
class FLIP_SIDE_API ATuto_Arrow : public AActor, public IShopTutorialInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATuto_Arrow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	class UShopTutorialWSubsystem* TutorialSubsystem;

	virtual void ExecuteTutorialEvent_Implementation() override;

	virtual void FinishTutorialEvent_Implementation() override;

private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	int32 Order= -1;
public:
	UFUNCTION(BlueprintCallAble)
	void CallEvent();
};
