// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/ItemLogicLibrary.h"
#include "Item_Action.h"
#include "GridActor.h"
#include "CoinActor.h"
#include "GridTypes.h"
#include "Component_Status.h"
#include "Base_OtherActor.h"
#include "BossActor.h"
#include "FlipSide_Enum.h"
#include "Components/StaticMeshComponent.h"
#include "OthersWSubsystem.h"
#include "GridManagerSubsystem.h"

namespace
{
    ACoinActor* GetFirstTargetCoin(UItem_Action* ItemContext)
    {
        if(!ItemContext) return nullptr;

        TArray<ACoinActor*> RangedCoins = ItemContext->GetInRangeCoins();
        if(RangedCoins.IsEmpty()) return nullptr;

        return RangedCoins[0];
    }
}

void UItemLogicLibrary::Test_Item(UItem_Action* ItemContext)
{
    UE_LOG(LogTemp, Warning, TEXT("Test Item"));
}
//보호막 융해 물약↓
void UItemLogicLibrary::MeltShieldPotion_Logic(UItem_Action* ItemContext)
{
    ACoinActor* TargetCoin = GetFirstTargetCoin(ItemContext);
    if(!TargetCoin || !TargetCoin->StatComponent) return;

    FBuffInfo Info;
    Info.BuffName = TEXT("보호막 융해물약");
    Info.PreGiveDelegate = FOnPreGiveDamage::FDelegate::CreateLambda([TargetCoin](AActor* Target, int32 FinalAttackPoint, int32& OutDmg)
    {
        ABossActor* Boss = Cast<ABossActor>(Target);
        if(!Boss || !Boss->HasShield()) return;

        Boss->ApplyShieldOnlyDamage(FinalAttackPoint * 3, TargetCoin);
    });

    TargetCoin->StatComponent->AddBuffs(Info);

    UE_LOG(LogTemp, Warning, TEXT("MeltShield"));
}
//피의 갈망 물약↓
void UItemLogicLibrary::BloodPotion_Logic(UItem_Action* ItemContext)
{
    ACoinActor* TargetCoin = GetFirstTargetCoin(ItemContext);
    if(!TargetCoin || !TargetCoin->StatComponent) return;

    UComponent_Status* TargetStat = TargetCoin->StatComponent;

    FBuffInfo Info;
    Info.BuffName = TEXT("피의 갈망 물약");
    Info.PostGiveDelegate = FOnPostGiveDamage::FDelegate::CreateLambda([TargetCoin, TargetStat](AActor* Target, int32 DealtHPDamage)
    {
        if(!Cast<ABossActor>(Target) || DealtHPDamage <= 0) return;

        TargetStat->ApplyHeal(DealtHPDamage, TargetCoin);
    });

    TargetCoin->StatComponent->AddBuffs(Info);

    UE_LOG(LogTemp, Warning, TEXT("Blood"));
}
//정화 물약↓
void UItemLogicLibrary::CleanserPotion_Logic(UItem_Action* ItemContext)
{
    ACoinActor* TargetCoin = GetFirstTargetCoin(ItemContext);
    if(!TargetCoin || !TargetCoin->StatComponent) return;

    TargetCoin->StatComponent->ClearDebuffs();
    TargetCoin->StatComponent->DecreaseCCDuration(TNumericLimits<int32>::Max());

    UE_LOG(LogTemp, Warning, TEXT("Cleanser"));
}
//위상변화 물약↓
void UItemLogicLibrary::PhaseChangePotion_Logic(UItem_Action* ItemContext)
{
    ACoinActor* TargetCoin = GetFirstTargetCoin(ItemContext);
    if(!TargetCoin) return;

    EFaceState NextFace = EFaceState::None;
    switch(TargetCoin->GetCoinDecidedFace())
    {
    case EFaceState::Front:
        NextFace = EFaceState::Back;
        break;
    case EFaceState::Back:
        NextFace = EFaceState::Front;
        break;
    default:
        return;
    }

    TargetCoin->SetCoinFace(NextFace);

    UE_LOG(LogTemp, Warning, TEXT("PhaseChange"));
}
//사방팔방 물약↓
void UItemLogicLibrary::EverwherePotion_Logic(UItem_Action* ItemContext)
{
    ACoinActor* TargetCoin = GetFirstTargetCoin(ItemContext);
    if(!ItemContext || !TargetCoin) return;

    AGridActor* TargetGrid = ItemContext->GetTargetGrid();
    if(!TargetGrid || TargetGrid->GetIsOccupied()) return;

    UWorld* World = TargetCoin->GetWorld();
    if(!World) return;

    UGridManagerSubsystem* GridManager = World->GetSubsystem<UGridManagerSubsystem>();
    if(!GridManager) return;

	const FGridPoint TargetPoint = TargetGrid->GetGridPoint();
	if (!GridManager->CanCoinOccupyCell(TargetPoint)) return;

    AGridActor* PrevGrid = GridManager->GetGridActor(TargetCoin->GetDecidedGrid());
    if(!PrevGrid) return;

	// 먼저 새 셀 점유에 성공한 뒤 기존 셀을 비워, 실패 시 코인이 점유 셀을 잃지 않게 합니다.
	if (!GridManager->TryOccupyCoinCell(TargetPoint, TargetCoin)) return;

    PrevGrid->ClearOccupiedOnly();

    const FVector TargetLocation = FVector(
        TargetGrid->GetGridWorldXY().X,
        TargetGrid->GetGridWorldXY().Y,
        TargetCoin->GetActorLocation().Z
    );

    TargetCoin->SetGridPoint(TargetPoint);
    TargetCoin->SetActorLocation(TargetLocation);

    UE_LOG(LogTemp, Warning, TEXT("Everywhere"));
}
//융기 물약↓
void UItemLogicLibrary::WallPotion_Logic(UItem_Action* ItemContext)
{
    if(!ItemContext) return;

    AGridActor* TargetGrid = ItemContext->GetTargetGrid();
    if(!TargetGrid) return;

    FString WallPath = TEXT("/Game/Others/BP_DefaultWall_OtherActor.BP_DefaultWall_OtherActor_C");
    UClass* WallClass = StaticLoadClass(ABase_OtherActor::StaticClass(), nullptr, *WallPath);

    UWorld* World = TargetGrid->GetWorld();
    if(World && WallClass)
    {
        FVector SpawnLocation = FVector(TargetGrid->GetGridWorldXY().X, TargetGrid->GetGridWorldXY().Y, 30.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        ABase_OtherActor* SpawnedWall = World->SpawnActor<ABase_OtherActor>(WallClass, SpawnLocation, SpawnRotation);
    
        if(SpawnedWall)
        {
            TargetGrid->SetOccupied(true, EGridOccupyingType::Wall, SpawnedWall);

            if(UOthersWSubsystem* OtherManager = World->GetSubsystem<UOthersWSubsystem>())
            {
                OtherManager->RegisterOther(SpawnedWall);
            }
        }
    }
}
