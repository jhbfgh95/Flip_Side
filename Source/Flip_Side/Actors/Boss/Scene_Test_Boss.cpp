#include "Scene_Test_Boss.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

AScene_Test_Boss::AScene_Test_Boss()
{
	BossStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	BossStaticMesh->SetupAttachment(RootComponent);

	if (BossMesh)
	{
		BossMesh->SetVisibility(false, true);
		BossMesh->SetHiddenInGame(true, true);
		BossMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ShieldEffectComponent)
	{
		ShieldEffectComponent->SetupAttachment(BossStaticMesh);
	}
}

void AScene_Test_Boss::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateSkeletalMeshVisibility();
}

void AScene_Test_Boss::BeginPlay()
{
	Super::BeginPlay();

	UpdateSkeletalMeshVisibility();
}

void AScene_Test_Boss::UpdateSkeletalMeshVisibility()
{
	if (!BossMesh)
	{
		return;
	}

	const bool bIsVer2 = GetClass()
		&& GetClass()->GetName().Contains(TEXT("ver2"), ESearchCase::IgnoreCase);
	const bool bShouldShowSkeletalMesh = bShowSkeletalMesh || bIsVer2;

	BossMesh->SetVisibility(bShouldShowSkeletalMesh, true);
	BossMesh->SetHiddenInGame(!bShouldShowSkeletalMesh, true);
}
