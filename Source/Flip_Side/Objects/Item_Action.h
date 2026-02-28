#pragma once

#include "CoreMinimal.h"
#include "Objects/ActionBase.h"
#include "Item_Action.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UItem_Action : public UActionBase
{
	GENERATED_BODY()
	
	int32 ItemEffectValue = 0;

	int32 ItemTypeID = 0;
	//흠.. 설치하는거 오브젝트를 어떻게 넣을지 고민해봐야하겠음

	class AGridActor* TargetGrid;

public:
	virtual void SetItemEffectValue(const int32 OnItemEffectValue);

	virtual int32 GetItemEffectValue() const;
	
	virtual void SetItemType(const int32 TypeID);

	virtual int32 GetItemTypeID() const;

	virtual void ExecuteAction() override;

	virtual void SetTargetGrid(class AGridActor* Grid);
};
