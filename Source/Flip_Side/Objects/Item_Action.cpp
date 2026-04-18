#include "Objects/Item_Action.h"
#include "Engine/World.h"
#include "DataManagerSubsystem.h"
#include "FlipSide_Enum.h"
#include "Subsystem/BattleLevel/ActionLogicRegistryGISubsystem.h"
void UItem_Action::SetItemEffectValue(const int32 OnItemEffectValue)
{
    ItemEffectValue = FMath::Max(0, OnItemEffectValue);
}

int32 UItem_Action::GetItemEffectValue() const
{
    return ItemEffectValue;
}

/*
void UItem_Action::SetItemType(const EItemType TypeID)
{
    ItemType = TypeID;
}

EItemType UItem_Action::GetItemTypeID() const
{
    return ItemType;
}
*/

void UItem_Action::SetTargetGrid(AGridActor* Grid)
{
    TargetGrid = Grid;
}

void UItem_Action::ExecuteAction()
{
    if(LogicID == -1) return;

    UWorld* World = GetWorld();
    if (!World) return;

    auto* DM = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if (DM && DM->IsCacheReady())
    {
        DM->TryGetItem(LogicID, ItemInfo);
    }

    UActionLogicRegistryGISubsystem* ActionRegistry = World->GetGameInstance()->GetSubsystem<UActionLogicRegistryGISubsystem>();
    if(!ActionRegistry) return;

    FActionLogic ItemLogic = ActionRegistry->GetItemLogic(this->LogicID);

    if(ItemLogic)
    {
        ItemLogic(this);
    }
}
