// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/WeaponLogicLibrary.h"
#include "Component_Status.h"
#include "Weapon_Action.h"
#include "CoinActor.h"
#include "BossActor.h"

void UWeaponLogicLibrary::Test_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Do"));
}

/* -- 딜러 -- */
//쇠파이프↓
void UWeaponLogicLibrary::SteelPipe_Logic(UWeapon_Action* WeaponContext)
{
    UE_LOG(LogTemp, Warning, TEXT("SteelPipe ON"));

    if(!WeaponContext) return;

    ABossActor* Boss;

    if(!WeaponContext->GetInRangeBoss(Boss)) return;

    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 BP = WeaponContext->GetFinalBehaviorPoint();

    int32 FinalDmg = AP * BP;

    UComponent_Status* TargetStat = Boss->GetStatusComponent();

    if(TargetStat)
    {
        TargetStat->ApplyDamage(FinalDmg, WeaponContext->GetCasterCoin());
    }

    UE_LOG(LogTemp, Warning, TEXT("SteelPipe ON"));
}
//증기 전기톱 ↓
void UWeaponLogicLibrary::SteamChainSaw_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    ABossActor* Boss;

    if(!WeaponContext->GetInRangeBoss(Boss)) return;

    int32 AP = WeaponContext->GetFinalAttackPoint();

    int32 FinalDmg = AP;

    UComponent_Status* TargetStat = Boss->GetStatusComponent();

    if(TargetStat)
    {
        TargetStat->ApplyDamage(FinalDmg, WeaponContext->GetCasterCoin());
    }

    UE_LOG(LogTemp, Warning, TEXT("Logic Chainsaw"));
}
//버거↓
void UWeaponLogicLibrary::Buger_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Buger"));
}
//혈사포↓
void UWeaponLogicLibrary::BloodCanon_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic BloodCanon"));
}
//자동터렛↓
void UWeaponLogicLibrary::AutoTurretSet_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic AutoTurret"));
}
//스나이퍼↓
void UWeaponLogicLibrary::SniperRifle_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Sniper"));
}

/* -- 탱커 -- */
//냉각기↓
void UWeaponLogicLibrary::Freezer_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Freezer"));
}
//연막슈트↓
void UWeaponLogicLibrary::SmokeSuit_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty()) return;

    UComponent_Status* TargetStat = nullptr;
    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 BP = WeaponContext->GetFinalBehaviorPoint();

    for(ACoinActor* Coin : RangedCoins)
    {
        TargetStat = Coin->StatComponent;

        FBuffInfo Info;
        Info.BuffName = TEXT("연막슈트");

        Info.DamageDelegate = FOnPreTakeDamage::FDelegate::CreateLambda([TargetStat, AP, BP](int32 InDmg, int32& OutDmg, bool& bIsIgnore)
        {
            float Chance = AP * 40.0f;
            if(FMath::RandRange(1, 100) <= Chance)
            {
                float DmgReduction = FMath::Clamp(((BP * 20) / 100.0f), 0.0f, 1.0f);
                OutDmg = FMath::RoundToInt(OutDmg * (1.0f - DmgReduction));

                UE_LOG(LogTemp, Warning, TEXT("연막슈트 발동"))
            }
        });

        TargetStat->AddBuffs(Info);
    }   

    UE_LOG(LogTemp, Warning, TEXT("연막슈트 적용"));
}
//장갑슈트↓
void UWeaponLogicLibrary::ArmorSuit_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty()) return;

    UComponent_Status* TargetStat = nullptr;
    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 BP = WeaponContext->GetFinalBehaviorPoint();

    for(ACoinActor* Coin : RangedCoins)
    {
        TargetStat = Coin->StatComponent;

        FBuffInfo Info;
        Info.BuffName = TEXT("장갑슈트");

        Info.DamageDelegate = FOnPreTakeDamage::FDelegate::CreateLambda([TargetStat, AP, BP](int32 InDmg, int32& OutDmg, bool& bIsIgnore)
        {
            float Chance = AP * 33.0f;
            if(FMath::RandRange(1, 100) <= Chance)
            {
                float DmgReduction = FMath::Clamp(((BP * 50) / 100.0f), 0.0f, 1.0f);
                OutDmg = FMath::RoundToInt(OutDmg * (1.0f - DmgReduction));

                UE_LOG(LogTemp, Warning, TEXT("장갑슈트 발동"))
            }
        });

        TargetStat->AddBuffs(Info);
    }

    UE_LOG(LogTemp, Warning, TEXT("장갑슈트 적용"));
}
//창의적↓
void UWeaponLogicLibrary::EnemyOfSpear_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic EOS"));
}
//건틀릿↓
void UWeaponLogicLibrary::Gauntlet_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Gauntlet"));
}

/* -- 유틸 -- */
//응급처치↓
void UWeaponLogicLibrary::Medikit_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty()) return;

    UComponent_Status* TargetStat = nullptr;
    int32 AP = WeaponContext->GetFinalAttackPoint();

    TargetStat = RangedCoins[0]->StatComponent;

    //이거 nullptr 나중에 WeaponAction에서 BatttleManager에서 Coinactionmanager에 넣어줘야해
    TargetStat->ApplyHeal(AP, nullptr);
}
//십자보호막↓
void UWeaponLogicLibrary::CrossShiled_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Gauntlet"));
}
//아드레날린↓
void UWeaponLogicLibrary::Adrenaline_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Gauntlet"));
}
//증폭조준↓
void UWeaponLogicLibrary::LockOnLenz_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Gauntlet"));
}
//긴급소생↓
void UWeaponLogicLibrary::Emergencylifer_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty()) return;

    UComponent_Status* TargetStat = nullptr;
    int32 AP = WeaponContext->GetFinalAttackPoint();

    TargetStat = RangedCoins[0]->StatComponent;

    FBuffInfo Info;
    Info.BuffName = TEXT("긴급소생장치");

    Info.DamageDelegate = FOnPreTakeDamage::FDelegate::CreateLambda([TargetStat, AP](int32 InDmg, int32& OutDmg, bool& bIsIgnore)
    {
        if(OutDmg >= TargetStat->GetHP())
        {
            bIsIgnore = true;
            TargetStat->SetHP(AP, false);

            UE_LOG(LogTemp, Warning, TEXT("죽을뻔하다 살아남"));
        }
        
    });

    TargetStat->AddBuffs(Info);

    UE_LOG(LogTemp, Warning, TEXT("긴급소생 적용"));
}
//드릴↓
void UWeaponLogicLibrary::Drill_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Gauntlet"));
}
//수리킷↓
void UWeaponLogicLibrary::Fixkit_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Gauntlet"));
}