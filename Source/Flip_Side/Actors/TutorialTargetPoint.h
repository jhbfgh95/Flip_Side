// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "TutorialTargetPoint.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API ATutorialTargetPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FName FocusId = NAME_None;
};
