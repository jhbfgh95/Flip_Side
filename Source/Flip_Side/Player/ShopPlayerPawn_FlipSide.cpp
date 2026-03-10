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

		ShopGameMode->OnCoinManageMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveCoinManageMode);
		
		ShopGameMode->OnSelectCardMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveSelectCardMode);
		
		ShopGameMode->OnShopItemMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveShopItemMode);

		
		ShopGameMode->OnShopMainMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveShopMainMode);

		ShopGameMode->OnUnlockWeaponMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveUnlockWeaponMode);
		
		ShopGameMode->OnUnlockCardMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveUnlockCardMode);
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


void AShopPlayerPawn_FlipSide::MoveCoinManageMode()
{
	SetActorLocation(CoinManageLocation);
}

void AShopPlayerPawn_FlipSide::MoveSelectCardMode()
{
	SetActorLocation(SelectCardLocation);
}

void AShopPlayerPawn_FlipSide::MoveShopItemMode()
{
	SetActorLocation(ShopItemLocation);
}


void AShopPlayerPawn_FlipSide::MoveShopMainMode()
{
	SetActorLocation(ShopMainLocation);
}

void AShopPlayerPawn_FlipSide::MoveUnlockWeaponMode()
{
	SetActorLocation(UnlockWeaponLocation);
}

void AShopPlayerPawn_FlipSide::MoveUnlockCardMode()
{
	SetActorLocation(UnlockCardLocation);
}