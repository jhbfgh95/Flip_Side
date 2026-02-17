// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/ItemDataTypes.h"
#include "ShopUseableItemActor.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API AShopUseableItemActor : public AActor
{
	GENERATED_BODY()

	AShopUseableItemActor();
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UseableItemRootComp;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* UseableItemMesh;

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
	//소모품이 우클릭됬을 때

	UFUNCTION()
	void RClickedUseAbleItem();
	//소모품위에 마우스가 올라왔을 때

	UFUNCTION()
	void HoveredUseAbleItem();

	//소모품위에 마우스가 내려 갔을 때
	UFUNCTION()
	void UnHoveredUseAbleItem();

	void SetItemMaterial();
};
