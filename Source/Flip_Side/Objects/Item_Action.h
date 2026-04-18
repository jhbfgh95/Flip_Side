#pragma once

#include "CoreMinimal.h"
#include "Objects/ActionBase.h"
#include "ItemDataTypes.h"
#include "Item_Action.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UItem_Action : public UActionBase
{
	GENERATED_BODY()
	
	int32 ItemEffectValue = 0;

	/* //현재 아이템 자체가 타입을 가져서 클릭에서 분기하기 때문에 딱히 필요없을 것으로 생각.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EItemType ItemType = EItemType::CoinBuff;
	*/
	class AGridActor* TargetGrid = nullptr;

	FItemData ItemInfo;

public:
	virtual void SetItemEffectValue(const int32 OnItemEffectValue);

	virtual int32 GetItemEffectValue() const;
	
	/*
	virtual void SetItemType(const EItemType TypeID);
	virtual EItemType GetItemTypeID() const;
	*/
	virtual void ExecuteAction() override;

	virtual void SetTargetGrid(class AGridActor* Grid);

	AGridActor* GetTargetGrid() const { return TargetGrid; }
};
