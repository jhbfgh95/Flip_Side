// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/ItemDataTypes.h"
#include "Interface/ShopMouseInterface.h"
#include "ShopUseableItemActor.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AShopUseableItemActor : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()

	AShopUseableItemActor();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UseableItemRootComp;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* UseableItemMesh;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemExplainMesh;
	
	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* InteractSphere;

	
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* ItemMeshTimeline;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* ItemBuyTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemMoveCurve;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemBuyCurve;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector ItemMoveDirection;

	FVector StartLocation;
	FVector ArriveLocation;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FRotator ItemShakeRotator;

	FRotator StartRotator;
	FRotator ArriveRotator;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector ItemDescriptionMoveDirection;

	FVector ItemDescriptionStartLocation;
	FVector ItemDescriptionArriveLocation;

private:

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
	int32 ShopItemIndex;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"))
    FItemData ShopItemData;
private:
	class UShopItemWSubsystem* ShopItemSubSystem;

private:
	//상호작용했을 떄 타임라인 추가.

public:
	//소모품이 좌 클릭됬을 때
	UFUNCTION()
	void LClickedUseAbleItem();

	UFUNCTION()
	void HoveredUseAbleItem();

	//소모품위에 마우스가 내려 갔을 때
	UFUNCTION()
	void UnHoveredUseAbleItem();

	void SetItemMaterial();
private:
	UFUNCTION()
	void ItemHoverMovement(float Value);
	UFUNCTION()
	void ItemBuyMovement(float Value);

public:
	UFUNCTION()
	void HoverdItem();
	UFUNCTION()
	void UnHoverdItem();

public:
	virtual void InteractHover_Implementation() override;
	virtual void InteractUnHover_Implementation() override;
	virtual void InteractLeftClick_Implementation() override;
};
