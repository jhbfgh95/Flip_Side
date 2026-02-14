// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UseableItemActor.generated.h"

UCLASS()
class AUseableItemActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UseableItemRootComp;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* UseableItemMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "UseableItem | Type")
	int32 ItemID = 0;	
	
	UPROPERTY(VisibleAnywhere)
	UTexture2D* ItemTexture;

	UPROPERTY(VisibleAnywhere)
	FLinearColor ItemColor;

public:	
	AUseableItemActor();

	int32 GetItemID() const;

	void SetItemValues(int TheItemID, UTexture2D* ItemTex, FLinearColor Color);
protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void Tick(float DeltaTime) override;

};
