#include "BossPillarActor.h"
#include "BossActor.h"
#include "GridManagerSubsystem.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

ABossPillarActor::ABossPillarActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	PillarMesh->SetupAttachment(RootComponent);
	// 명중은 메시 충돌 대신 점유 그리드로 판정합니다.
	PillarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PillarMesh->SetGenerateOverlapEvents(false);
}

void ABossPillarActor::InitializeForBoss(ABossActor* InBoss)
{
	OwningBoss = InBoss;
	SetOwner(InBoss);
}

void ABossPillarActor::SetGridPlacement(const TArray<FGridPoint>& InCells, const FVector& WorldLocation)
{
	OccupiedCells = InCells;
	SetActorLocation(WorldLocation);
	UWorld* World = GetWorld();
	UGridManagerSubsystem* GridManager = IsValid(World) ? World->GetSubsystem<UGridManagerSubsystem>() : nullptr;
	if (IsValid(GridManager)) GridManager->RegisterBossPillar(this);
}

ABossActor* ABossPillarActor::GetOwningBoss() const
{
	return OwningBoss.Get();
}

int32 ABossPillarActor::GetFootprintSize() const
{
	return 2;
}

int32 ABossPillarActor::GetCurrentHP() const
{
	const ABossActor* Boss = GetOwningBoss();
	return IsValid(Boss) ? Boss->GetCurrentHP() : 0;
}

int32 ABossPillarActor::GetCurrentShield() const
{
	const ABossActor* Boss = GetOwningBoss();
	return IsValid(Boss) ? Boss->GetCurrentShield() : 0;
}

int32 ABossPillarActor::ApplyBossDamage(int32 Damage, AActor* DamageCauser)
{
	ABossActor* Boss = GetOwningBoss();
	return IsValid(Boss) && IsValid(DamageCauser)
		? Boss->ApplyDamageAndReturnHPDamage(Damage, DamageCauser) : 0;
}

float ABossPillarActor::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	return static_cast<float>(ApplyBossDamage(FMath::RoundToInt(FMath::Max(0.f, DamageAmount)), DamageCauser));
}

void ABossPillarActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OccupiedCells.Reset();
	UWorld* World = GetWorld();
	UGridManagerSubsystem* GridManager = IsValid(World) ? World->GetSubsystem<UGridManagerSubsystem>() : nullptr;
	if (IsValid(GridManager)) GridManager->UnregisterBossPillar(this);
	OwningBoss.Reset();
	Super::EndPlay(EndPlayReason);
}
