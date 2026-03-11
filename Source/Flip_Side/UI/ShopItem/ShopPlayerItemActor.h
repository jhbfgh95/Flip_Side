// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/ItemDataTypes.h"
#include "Interface/ShopMouseInterface.h"
#include "ShopPlayerItemActor.generated.h"

UCLASS()
class FLIP_SIDE_API AShopPlayerItemActor : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopPlayerItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UseableItemRootComp;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemDescriptionMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* InteractSphere;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ItemWidget;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* ItemMeshTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemMoveCurve;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* DescriptionCurve;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector ItemMoveDirection;

	FVector StartLocation;
	FVector ArriveLocation;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector DescriptionMoveDirection;

	FVector DescriptionStartLocation;
	FVector DescriptionArriveLocation;
public:
	
	UPROPERTY(EditAnywhere)
	int32 ItemIndex;
private:

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
    FItemData PlayerItemData;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
    FSelectItem PlayerSelectItemData;
private:
	class UShopItemWSubsystem* ShopItemSubsystem;
	
	class UDataManagerSubsystem* ItemDataSubsystem;



private:
	UFUNCTION()
	void ItemHoverMovement(float Value);

	UFUNCTION()
	void DescriptionMovement(float Value);

public:
	
	UFUNCTION()
	void HoverdItem();
	UFUNCTION()
	void UnHoverdItem();
	
	void InitItem(int32 Index);

	void SetItemMaterial();

	void HideItem();

public:
	virtual void InteractHover_Implementation() override;
	virtual void InteractUnHover_Implementation() override;
};
