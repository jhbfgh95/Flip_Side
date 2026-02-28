#include "Objects/Item_Action.h"
#include "Subsystem/BattleLevel/ActionLogicRegistryGISubsystem.h"
void UItem_Action::SetItemEffectValue(const int32 OnItemEffectValue)
{
    ItemEffectValue = FMath::Max(0, OnItemEffectValue);
}

int32 UItem_Action::GetItemEffectValue() const
{
    return ItemEffectValue;
}

void UItem_Action::SetItemType(const int32 TypeID)
{
    ItemTypeID = TypeID;
}

int32 UItem_Action::GetItemTypeID() const
{
    return ItemTypeID;
}

void UItem_Action::SetTargetGrid(AGridActor* Grid)
{
    if(!TargetGrid) return;

    TargetGrid = Grid;
}

void UItem_Action::ExecuteAction()
{
    if(LogicID == -1) return;

    UWorld* World = GetWorld();
    if (!World) return;

    UActionLogicRegistryGISubsystem* ActionRegistry = World->GetGameInstance()->GetSubsystem<UActionLogicRegistryGISubsystem>();
    if(!ActionRegistry) return;

    FActionLogic ItemLogic = ActionRegistry->GetItemLogic(this->LogicID);

    if(ItemLogic)
    {
        ItemLogic(this);
    }
}