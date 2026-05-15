// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPreviewActor.generated.h"

UCLASS()
class FLIP_SIDE_API AItemPreviewActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ItemPreview | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* PreviewRootComp;

	UPROPERTY(EditAnywhere, Category = "ItemPreview | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PreviewMesh;

	UPROPERTY(EditAnywhere, Category = "ItemPreview | Scale", meta = (AllowPrivateAccess = "true"))
	float PreviewScaleMultiplier = 0.6f;
	
public:	
	AItemPreviewActor();

	void InitFromItem(class AUseableItemActor* SourceItem);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
