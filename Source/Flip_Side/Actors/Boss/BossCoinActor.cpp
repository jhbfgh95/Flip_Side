#include "Actors/Boss/BossCoinActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Subsystem/BossSetupGISubsystem.h"

ABossCoinActor::ABossCoinActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BossCoinRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = BossCoinRoot;

	BossCoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boss Coin Mesh"));
	BossCoinMesh->SetupAttachment(RootComponent);
	BossCoinMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
}

void ABossCoinActor::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GI = GetGameInstance();
	if (!IsValid(BossCoinMesh) || !IsValid(GI)) return;

	UBossSetupGISubsystem* BossSetup = GI->GetSubsystem<UBossSetupGISubsystem>();
	if (!IsValid(BossSetup)) return;

	FBossDisplayData BossData;
	if (!BossSetup->GetPreparedBossData(BossData) || !IsValid(BossData.BossIcon))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossCoinActor] Prepared boss data or BossIcon is missing."));
		return;
	}

	const FName IconParameterName(TEXT("Boss_Icon"));
	bool bAppliedIcon = false;
	for (int32 MaterialIndex = 0; MaterialIndex < BossCoinMesh->GetNumMaterials(); ++MaterialIndex)
	{
		UMaterialInterface* Material = BossCoinMesh->GetMaterial(MaterialIndex);
		if (!IsValid(Material)) continue;

		UTexture* ExistingTexture = nullptr;
		if (!Material->GetTextureParameterValue(FMaterialParameterInfo(IconParameterName), ExistingTexture)) continue;

		UMaterialInstanceDynamic* DynamicMaterial = BossCoinMesh->CreateDynamicMaterialInstance(MaterialIndex);
		if (!IsValid(DynamicMaterial)) continue;

		DynamicMaterial->SetTextureParameterValue(IconParameterName, BossData.BossIcon.Get());
		bAppliedIcon = true;
	}

	if (!bAppliedIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossCoinActor] Could not apply Boss_Icon (BossID=%d)."), BossData.BossID);
	}
}
