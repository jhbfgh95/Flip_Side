#include "Subsystem/BattleLevel/ActionLogicRegistryGISubsystem.h"
#include "WeaponLogicLibrary.h"
#include "ItemLogicLibrary.h"
#include "Weapon_Action.h"
#include "Item_Action.h"

void UActionLogicRegistryGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    RegisterWeaponLogic();
    RegisterBossLogic();
    RegisterItemLogic();
}

void UActionLogicRegistryGISubsystem::RegisterWeaponLogic()
{
    /*모든 함수를 이렇게 매칭시킴*/
    WeaponMap.Add(1, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Test_Logic(WeaponAction);
        }
    });
}

void UActionLogicRegistryGISubsystem::RegisterBossLogic()
{

}

void UActionLogicRegistryGISubsystem::RegisterItemLogic()
{
    ItemMap.Add(1, [](UActionBase* Context) {
        if (UItem_Action* ItemAction = Cast<UItem_Action>(Context)) {
            UItemLogicLibrary::Test_Item(ItemAction);
        }
    });
}