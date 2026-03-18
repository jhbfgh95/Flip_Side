#include "ShopPlayerPawn_FlipSide.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/GameMode_Shop.h"

AShopPlayerPawn_FlipSide::AShopPlayerPawn_FlipSide()
{
	PrimaryActorTick.bCanEverTick = false;

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


void AShopPlayerPawn_FlipSide::SetCameraOrthographic()
{
	if(Camera->ProjectionMode == ECameraProjectionMode::Orthographic)
		return;
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 700;
}

void AShopPlayerPawn_FlipSide::SetCameraPerspective()
{
	if(Camera->ProjectionMode == ECameraProjectionMode::Perspective)
		return;
	Camera->ProjectionMode = ECameraProjectionMode::Perspective;
}


void AShopPlayerPawn_FlipSide::MoveCoinCreateMode()
{
	SetActorLocation(CoinCreateLocation);
	SetCameraPerspective();
}


void AShopPlayerPawn_FlipSide::MoveCoinManageMode()
{
	SetActorLocation(CoinManageLocation);
	SetCameraOrthographic();
}

void AShopPlayerPawn_FlipSide::MoveSelectCardMode()
{
	SetActorLocation(SelectCardLocation);
	SetCameraOrthographic();
}

void AShopPlayerPawn_FlipSide::MoveShopItemMode()
{
	SetActorLocation(ShopItemLocation);
	SetCameraOrthographic();
}


void AShopPlayerPawn_FlipSide::MoveShopMainMode()
{
	SetActorLocation(ShopMainLocation);
	SetCameraOrthographic();
}

void AShopPlayerPawn_FlipSide::MoveUnlockWeaponMode()
{
	SetActorLocation(UnlockWeaponLocation);
	SetCameraPerspective();
}

void AShopPlayerPawn_FlipSide::MoveUnlockCardMode()
{
	SetActorLocation(UnlockCardLocation);
	SetCameraOrthographic();
}