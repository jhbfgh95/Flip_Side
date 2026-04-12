// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/WeaponLogicLibrary.h"
#include "Component_Status.h"
#include "Weapon_Action.h"
#include "CoinActor.h"
#include "BossActor.h"
#include "GridActor.h"
#include "Turret_OtherActor.h"

void UWeaponLogicLibrary::Test_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    UE_LOG(LogTemp, Warning, TEXT("Logic Do"));
}

/* -- 딜러 -- */
//쇠파이프↓
void UWeaponLogicLibrary::SteelPipe_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    ABossActor* Boss;

    if(!WeaponContext->GetInRangeBoss(Boss) || !WeaponContext->GetCasterCoin()) return;

    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 BP = WeaponContext->GetFinalBehaviorPoint();

    int32 FinalDmg = AP * BP;

    if(Boss)
    {
        Boss->ApplyDamage(FinalDmg, WeaponContext->GetCasterCoin());
    }


    UE_LOG(LogTemp, Warning, TEXT("SteelPipe ON"));
}
//증기 전기톱 ↓
void UWeaponLogicLibrary::SteamChainSaw_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    ABossActor* Boss;

    if(!WeaponContext->GetInRangeBoss(Boss) || !WeaponContext->GetCasterCoin()) return;

    int32 AP = WeaponContext->GetFinalAttackPoint();

    int32 FinalDmg = AP;

    if(Boss)
    {
        Boss->ApplyDamage(FinalDmg, WeaponContext->GetCasterCoin());
    }

    UE_LOG(LogTemp, Warning, TEXT("Logic Chainsaw"));
}
//버거↓
void UWeaponLogicLibrary::Buger_Logic(UWeapon_Action* WeaponContext)
{

}
//혈사포↓
void UWeaponLogicLibrary::BloodCanon_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    ABossActor* Boss;

    if(!WeaponContext->GetInRangeBoss(Boss) || !WeaponContext->GetCasterCoin()) return;

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty()) return;

    UComponent_Status* TargetStat = nullptr;
    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 BP = WeaponContext->GetFinalBehaviorPoint();

    for(ACoinActor* Coin : RangedCoins)
    {
        if(Coin == WeaponContext->GetCasterCoin()) continue;
        TargetStat = Coin->StatComponent;

        TargetStat->ApplyDamage(AP, WeaponContext->GetCasterCoin());
        AP += BP;
    }

    if(Boss)
    {
        Boss->ApplyDamage(AP, WeaponContext->GetCasterCoin());
    }
}
//자동터렛↓
void UWeaponLogicLibrary::AutoTurretSet_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    AGridActor* TargetGrid = WeaponContext->GetTargetGrid();
    ACoinActor* CasterCoin = WeaponContext->GetCasterCoin();

    if (!TargetGrid) return;
    
    FString TurretPath = TEXT("/Game/Others/BP_Turret_OtherActor.BP_Turret_OtherActor_C");
    UClass* TurretClass = StaticLoadClass(ATurret_OtherActor::StaticClass(), nullptr, *TurretPath);
    
    UWorld* World = TargetGrid->GetWorld();
    if (World  && TurretClass)
    {
        FVector SpawnLocation = FVector(TargetGrid->GetGridWorldXY().X, TargetGrid->GetGridWorldXY().Y, -80.f); 
        FRotator SpawnRotation = FRotator::ZeroRotator; 

        ATurret_OtherActor* SpawnedActor = World->SpawnActor<ATurret_OtherActor>(TurretClass, SpawnLocation, SpawnRotation);

        if (SpawnedActor)
        {
            TargetGrid->SetOccupied(true, EGridOccupyingType::Turret, SpawnedActor);
            SpawnedActor->SetTurretSpawnGrid(TargetGrid->GetGridPoint());
            SpawnedActor->SetTurretAttackPoint(WeaponContext->GetFinalAttackPoint());
        }
    }
    
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

    if(!WeaponContext->GetCasterCoin()) return;

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty()) return;

    //코인들 버프 줄 스텟 컴포넌트
    UComponent_Status* TargetStat = nullptr;
    //자신 데미지
    UComponent_Status* MyStat = WeaponContext->GetCasterCoin()->StatComponent;
    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 BP = WeaponContext->GetFinalBehaviorPoint();

    /* 버프를 줄 코인들의 개수 (기획서)*/
    int32 BuffCoinNum = 0;

    if(RangedCoins.Num() > (AP + 2)) BuffCoinNum = RangedCoins.Num();
    else BuffCoinNum = AP + 2;

    /* 루프 */
    int32 i = 0;

    /* 자신에게 데미지 로직 버프 걸어둠 */
    FBuffInfo MyInfo;
    MyInfo.BuffName = TEXT("창의 적");
    MyInfo.DamageDelegate = FOnPreTakeDamage::FDelegate::CreateLambda([MyStat, AP, BuffCoinNum](int32 InDmg, int32& OutDmg, bool& bIsIgnore)
    {
        float DmgReduction =  FMath::Clamp(BuffCoinNum * (95 - ((AP * 10)/ 100.0f)* InDmg) + InDmg, 0, 100.0f);
        OutDmg = FMath::RoundToInt(DmgReduction);
    });

    for(ACoinActor* Coin : RangedCoins)
    {
        if(i >= BuffCoinNum) break;
        TargetStat = Coin->StatComponent;
        if(Coin == WeaponContext->GetCasterCoin()) continue;

        FBuffInfo Info;
        Info.BuffName = TEXT("창의 적");

        Info.DamageDelegate = FOnPreTakeDamage::FDelegate::CreateLambda([TargetStat, AP](int32 InDmg, int32& OutDmg, bool& bIsIgnore)
        {

            float DmgReduction = FMath::Clamp(((AP * 12) / 100.0f), 0.0f, 1.0f);

            OutDmg = FMath::RoundToInt(OutDmg * (1.0f - DmgReduction));
            UE_LOG(LogTemp, Warning, TEXT("창의적 발동"));

        });

        TargetStat->AddBuffs(Info);
        i++;
    }   

    UE_LOG(LogTemp, Warning, TEXT("창의적 적용"));

}
//건틀릿↓
void UWeaponLogicLibrary::Gauntlet_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    ABossActor* Boss;
    if(!WeaponContext->GetInRangeBoss(Boss) || !WeaponContext->GetCasterCoin()) return;

    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 BP = WeaponContext->GetFinalBehaviorPoint();

    float Chance = AP + 1;
    float Death = BP * 10;
    if(FMath::RandRange(1, 100) <= Chance)
    {
        //int 최대치
        Boss->ApplyDamage(TNumericLimits<int32>::Max(), WeaponContext->GetCasterCoin());
        return;
    }
    
    if(FMath::RandRange(1, 100) <= Death)
    {
        WeaponContext->GetCasterCoin()->StatComponent->OnDead.Broadcast();
    }
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

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty() || !WeaponContext->GetCasterCoin()) return;

    UComponent_Status* TargetStat = nullptr;
    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 BP = WeaponContext->GetFinalBehaviorPoint();

    for(ACoinActor* Coin : RangedCoins)
    {
        TargetStat = Coin->StatComponent;

        TargetStat->ApplyShield(AP, WeaponContext->GetCasterCoin());
    }   
    
}
//아드레날린↓
void UWeaponLogicLibrary::Adrenaline_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty()) return;

    UComponent_Status* TargetStat = nullptr;
    int32 AP = WeaponContext->GetFinalAttackPoint();

    TargetStat = RangedCoins[0]->StatComponent;

    FBuffInfo Info;
    Info.BuffName = TEXT("아드레날린 권총");

    Info.StatDelegate = FOnCalculateStats::FDelegate::CreateLambda([TargetStat, AP](FActionTask BuffTask)
    {
        BuffTask.ModifiedAttackPoint += AP;
    });

    TargetStat->AddBuffs(Info);
    
}
//증폭조준↓
void UWeaponLogicLibrary::LockOnLenz_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    TArray<ACoinActor*> RangedCoins = WeaponContext->GetInRangeCoins();
    if(RangedCoins.IsEmpty()) return;

    UComponent_Status* TargetStat = nullptr;
    int32 AP = WeaponContext->GetFinalAttackPoint();

    TargetStat = RangedCoins[0]->StatComponent;

    FBuffInfo Info;
    Info.BuffName = TEXT("증폭 조준 렌즈");

    Info.StatDelegate = FOnCalculateStats::FDelegate::CreateLambda([TargetStat, AP](FActionTask BuffTask)
    {
        BuffTask.ModifiedRange.GridX += AP;
        BuffTask.ModifiedRange.GridY += AP;
    });

    TargetStat->AddBuffs(Info);

    UE_LOG(LogTemp, Warning, TEXT("조준렌즈 적용"));
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