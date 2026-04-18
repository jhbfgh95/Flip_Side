// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/WeaponLogicLibrary.h"
#include "Component_Status.h"
#include "Weapon_Action.h"
#include "CoinActor.h"
#include "BossActor.h"
#include "GridActor.h"
#include "Base_OtherActor.h"
#include "Turret_OtherActor.h"
#include "AttackAreaTypes.h"
#include "CoinDataTypes.h"
#include "FlipSide_Enum.h"
#include "GridTypes.h"
#include "GridManagerSubsystem.h"
#include "OthersWSubsystem.h"

namespace
{
    void SetBuffIconFromWeapon(const UWeapon_Action* WeaponContext, FBuffInfo& BuffInfo)
    {
        if (WeaponContext)
        {
            BuffInfo.BuffIcon = WeaponContext->GetWeaponIcon();
        }
    }

    int32 ApplyBossDamageWithAttackerBuff(UWeapon_Action* WeaponContext, ABossActor* Boss, int32 FinalAttackPoint, int32 Damage)
    {
        if(!WeaponContext || !Boss || !WeaponContext->GetCasterCoin()) return 0;

        ACoinActor* CasterCoin = WeaponContext->GetCasterCoin();
        UComponent_Status* CasterStat = CasterCoin->StatComponent;

        int32 FinalDamage = Damage;
        if(CasterStat)
        {
            CasterStat->CheckAttackerPreBuff(Boss, FinalAttackPoint, FinalDamage);
        }

        const int32 DealtHPDamage = Boss->ApplyDamageAndReturnHPDamage(FinalDamage, CasterCoin);

        if(CasterStat)
        {
            CasterStat->CheckAttackerPostBuff(Boss, DealtHPDamage);
        }

        return DealtHPDamage;
    }
}

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

    ApplyBossDamageWithAttackerBuff(WeaponContext, Boss, AP, FinalDmg);


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

    ApplyBossDamageWithAttackerBuff(WeaponContext, Boss, AP, FinalDmg);

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

    ApplyBossDamageWithAttackerBuff(WeaponContext, Boss, AP, AP);
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
        FVector SpawnLocation = FVector(TargetGrid->GetGridWorldXY().X, TargetGrid->GetGridWorldXY().Y, -50.f); 
        FRotator SpawnRotation = FRotator::ZeroRotator; 

        ATurret_OtherActor* SpawnedActor = World->SpawnActor<ATurret_OtherActor>(TurretClass, SpawnLocation, SpawnRotation);

        if (SpawnedActor)
        {
            TargetGrid->SetOccupied(true, EGridOccupyingType::Turret, SpawnedActor);
            SpawnedActor->SetTurretSpawnGrid(TargetGrid->GetGridPoint());
            SpawnedActor->SetTurretAttackPoint(WeaponContext->GetFinalAttackPoint());

            if(UOthersWSubsystem* OtherManager = World->GetSubsystem<UOthersWSubsystem>())
            {
                OtherManager->RegisterOther(SpawnedActor);
            }
        }
    }
    
}
//스나이퍼↓
void UWeaponLogicLibrary::SniperRifle_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    ABossActor* Boss;

    if(!WeaponContext->GetInRangeBoss(Boss) || !WeaponContext->GetCasterCoin()) return;
    bool bIsSideOfWall = false;

    int32 AP = WeaponContext->GetFinalAttackPoint();
    int32 Range = WeaponContext->GetFinalRangeY();

    FAttackAreaSpec CheckSpec;
    FGridPoint CheckRange = {1,1};
    TArray<FGridPoint> CheckCells;
    FObjectOnGridInfo Info;
    CheckSpec.Pattern = EAttackAreaPattern::CrossOnCell;
    CheckSpec.AnchorMode = EAreaAnchor::UseAnchorCell;
    CheckSpec.AnchorCell = WeaponContext->GetCasterCoin()->GetDecidedGrid();
    CheckSpec.ParamA = 1;
    CheckSpec.ParamB = 1;

    if (UWorld* World = WeaponContext->GetWorld())
    {
        UGridManagerSubsystem* GM = World->GetSubsystem<UGridManagerSubsystem>();
        if(GM)
        {
            GM->GetObjectsAtRange(CheckSpec, CheckRange, CheckCells, Info);
        }
    }
    for(AActor* Other : Info.Others)
    {
        ABase_OtherActor* Wall = Cast<ABase_OtherActor>(Other);
        if(Wall->GetOtherType() == EOthersType::Wall)
        {
            bIsSideOfWall = true;
            break;
        }
    }

    if(Boss && !bIsSideOfWall)
    {
        ApplyBossDamageWithAttackerBuff(WeaponContext, Boss, AP, AP);
    }
    else if(Boss && bIsSideOfWall)
    {
        ApplyBossDamageWithAttackerBuff(WeaponContext, Boss, AP, AP * Range);
    }
}

/* -- 탱커 -- */
//냉각기↓
void UWeaponLogicLibrary::Freezer_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    ABossActor* Boss = nullptr;
    if(!WeaponContext->GetInRangeBoss(Boss) || !Boss) return;

    const int32 AP = WeaponContext->GetFinalAttackPoint();
    const int32 BP = WeaponContext->GetFinalBehaviorPoint();
    const float Chance = BP * 40.0f;

    if(FMath::RandRange(1, 100) <= Chance)
    {
        FCCStructure CC;
        CC.CCType = ECCTypes::Stun;
        CC.CCDuration = AP;

        Boss->ApplyCC(CC);

        UE_LOG(LogTemp, Warning, TEXT("급속 냉각기 스턴 적용"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("급속 냉각기 스턴 실패"));
    }
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
        SetBuffIconFromWeapon(WeaponContext, Info);

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
        SetBuffIconFromWeapon(WeaponContext, Info);

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
    SetBuffIconFromWeapon(WeaponContext, MyInfo);
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
        SetBuffIconFromWeapon(WeaponContext, Info);

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
        WeaponContext->GetCasterCoin()->StatComponent->ApplyDamage(TNumericLimits<int32>::Max(), WeaponContext->GetCasterCoin());
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
    SetBuffIconFromWeapon(WeaponContext, Info);

    Info.StatDelegate = FOnCalculateStats::FDelegate::CreateLambda([TargetStat, AP](FActionTask& BuffTask)
    {
        BuffTask.ModifiedAttackPoint += AP;
    });

    TargetStat->AddBuffs(Info);
    
    UE_LOG(LogTemp, Warning, TEXT("공격력 up"));
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
    SetBuffIconFromWeapon(WeaponContext, Info);

    Info.StatDelegate = FOnCalculateStats::FDelegate::CreateLambda([TargetStat, AP](FActionTask& BuffTask)
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
    SetBuffIconFromWeapon(WeaponContext, Info);

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

    ABase_OtherActor* TargetOther = WeaponContext->GetTargetOther();
    ACoinActor* CasterCoin = WeaponContext->GetCasterCoin();
    if(!TargetOther || !CasterCoin) return;

    TargetOther->ApplyDamage(WeaponContext->GetFinalAttackPoint(), CasterCoin);

    UE_LOG(LogTemp, Warning, TEXT("Logic Drill"));
}
//수리킷↓
void UWeaponLogicLibrary::Fixkit_Logic(UWeapon_Action* WeaponContext)
{
    if(!WeaponContext) return;

    ABase_OtherActor* TargetOther = WeaponContext->GetTargetOther();
    ACoinActor* CasterCoin = WeaponContext->GetCasterCoin();
    if(!TargetOther || !CasterCoin) return;

    TargetOther->ApplyHeal(WeaponContext->GetFinalAttackPoint(), CasterCoin);

    UE_LOG(LogTemp, Warning, TEXT("Logic Fixkit"));
}
