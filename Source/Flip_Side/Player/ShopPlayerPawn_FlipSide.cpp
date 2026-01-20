#include "ShopPlayerPawn_FlipSide.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/GameMode_Shop.h"

AShopPlayerPawn_FlipSide::AShopPlayerPawn_FlipSide()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 1500.0f;
	Camera->AutoPlaneShift = 1.0f;
	Camera->bUpdateOrthoPlanes = false;
}

void AShopPlayerPawn_FlipSide::BeginPlay()
{
	Super::BeginPlay();

	ShopGameMode = Cast<AGameMode_Shop>(GetWorld()->GetAuthGameMode());
	if(ShopGameMode)
	{
		ShopGameMode->OnCoinCreateMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveCoinCreateMode);
	}
}

void AShopPlayerPawn_FlipSide::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
void AShopPlayerPawn_FlipSide::MoveCoinCreateMode()
{
	SetActorLocation(CoinCreateLocation);
}
