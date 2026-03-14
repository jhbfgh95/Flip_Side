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
            UWeaponLogicLibrary::SteelPipe_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(2, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::SteamChainSaw_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(3, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Buger_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(4, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::BloodCanon_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(4, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::AutoTurretSet_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(6, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::SniperRifle_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(7, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Freezer_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(8, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::SmokeSuit_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(9, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::ArmorSuit_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(10, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::EnemyOfSpear_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(11, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Gauntlet_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(12, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Medikit_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(13, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::CrossShiled_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(14, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Adrenaline_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(15, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::LockOnLenz_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(16, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Emergencylifer_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(17, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Drill_Logic(WeaponAction);
        }
    });

    WeaponMap.Add(18, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Fixkit_Logic(WeaponAction);
        }
    });
}

void UActionLogicRegistryGISubsystem::RegisterBossLogic()
{

}

void UActionLogicRegistryGISubsystem::RegisterItemLogic()
{
    //보호막 융해로 바꿀것
    ItemMap.Add(1, [](UActionBase* Context) {
        if (UItem_Action* ItemAction = Cast<UItem_Action>(Context)) {
            UItemLogicLibrary::Test_Item(ItemAction);
        }
    });

    ItemMap.Add(2, [](UActionBase* Context) {
        if (UItem_Action* ItemAction = Cast<UItem_Action>(Context)) {
            UItemLogicLibrary::BloodPotion_Logic(ItemAction);
        }
    });

    ItemMap.Add(3, [](UActionBase* Context) {
        if (UItem_Action* ItemAction = Cast<UItem_Action>(Context)) {
            UItemLogicLibrary::CleanserPotion_Logic(ItemAction);
        }
    });

    ItemMap.Add(4, [](UActionBase* Context) {
        if (UItem_Action* ItemAction = Cast<UItem_Action>(Context)) {
            UItemLogicLibrary::PhaseChangePotion_Logic(ItemAction);
        }
    });

    ItemMap.Add(5, [](UActionBase* Context) {
        if (UItem_Action* ItemAction = Cast<UItem_Action>(Context)) {
            UItemLogicLibrary::EverwherePotion_Logic(ItemAction);
        }
    });

}