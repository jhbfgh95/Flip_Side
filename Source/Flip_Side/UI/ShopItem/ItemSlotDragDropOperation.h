// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemSlotDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UItemSlotDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:
    UPROPERTY()
    TObjectPtr<class UUserWidget> DraggedSlotWidget;
	
};
