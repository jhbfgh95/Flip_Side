#include "Subsystem/BattleLevel/ActionLogicRegistryGISubsystem.h"
#include "WeaponLogicLibrary.h"
#include "ItemLogicLibrary.h"
#include "Weapon_Action.h"
#include "Item_Action.h"

void UActionLogicRegistryGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    RegisterWeaponLogic();
    RegisterItemLogic();
}

void UActionLogicRegistryGISubsystem::RegisterWeaponLogic()
{
    /*모든 함수를 이렇게 매칭시킴*/
    WeaponMap.Add(-3, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::EmptyFallbackFront_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(-3, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveBossDamage(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(-2, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::EmptyFallbackBack_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(-2, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveBossDamage(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(1, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::SteelPipe_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(1, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveBossDamage(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(2, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::SteamChainSaw_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(2, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveBossDamage(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(3, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Buger_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(3, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(4, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::BloodCanon_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(4, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveBossDamageWithCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(5, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::AutoTurretSet_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(5, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveGridTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(6, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::SniperRifle_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(6, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveBossDamage(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(7, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Freezer_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(7, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveBossCC(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(8, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::SmokeSuit_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(8, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(9, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::ArmorSuit_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(9, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(10, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::EnemyOfSpear_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(10, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(11, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Gauntlet_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(11, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveBossDamage(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(12, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Medikit_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(12, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(13, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::CrossShiled_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(13, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(14, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Adrenaline_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(14, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(15, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::LockOnLenz_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(15, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(16, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Emergencylifer_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(16, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveCoinTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(17, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Drill_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(17, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveOtherTarget(Cast<UWeapon_Action>(Context)); });

    WeaponMap.Add(18, [](UActionBase* Context) {
        if (UWeapon_Action* WeaponAction = Cast<UWeapon_Action>(Context)) {
            UWeaponLogicLibrary::Fixkit_Logic(WeaponAction);
        }
    });
    WeaponResolveMap.Add(18, [](UActionBase* Context) { return UWeaponLogicLibrary::ResolveOtherTarget(Cast<UWeapon_Action>(Context)); });
}

void UActionLogicRegistryGISubsystem::RegisterItemLogic()
{
    //보호막 융해로 바꿀것
    ItemMap.Add(1, [](UActionBase* Context) {
        if (UItem_Action* ItemAction = Cast<UItem_Action>(Context)) {
            UItemLogicLibrary::MeltShieldPotion_Logic(ItemAction);
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
            UItemLogicLibrary::WallPotion_Logic(ItemAction);
        }
    });

    ItemMap.Add(6, [](UActionBase* Context) {
        if (UItem_Action* ItemAction = Cast<UItem_Action>(Context)) {
            UItemLogicLibrary::EverwherePotion_Logic(ItemAction);
        }
    });
}
