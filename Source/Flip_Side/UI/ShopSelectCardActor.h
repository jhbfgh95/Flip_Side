// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopSelectCardActor.generated.h"

UCLASS()
class FLIP_SIDE_API AShopSelectCardActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopSelectCardActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* CardWidget;
	
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UUserWidget* CardWidgetClass;
	
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CardMesh;
};
