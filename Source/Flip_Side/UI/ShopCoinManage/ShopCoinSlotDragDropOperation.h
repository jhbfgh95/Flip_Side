#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ShopCoinSlotDragDropOperation.generated.h"

UCLASS()
class FLIP_SIDE_API UShopCoinSlotDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 SourceSlotIndex = INDEX_NONE;

	UPROPERTY()
	int32 TargetSlotIndex = INDEX_NONE;
};
