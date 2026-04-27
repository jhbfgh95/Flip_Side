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
		ShopGameMode->OnCoinManageMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveCoinManageMode);
		
		ShopGameMode->OnSelectCardMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveSelectCardMode);
		
		ShopGameMode->OnShopItemMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveShopItemMode);

		
		ShopGameMode->OnShopMainMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveShopMainMode);

		ShopGameMode->OnUnlockWeaponMode.AddDynamic(this, &AShopPlayerPawn_FlipSide::MoveUnlockWeaponMode);
	}
	
	SetActorLocation(ShopMainLocation);
	SetActorRotation(MainRotation);
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

void AShopPlayerPawn_FlipSide::MoveCoinManageMode()
{
	SetActorRotation(FRotator::ZeroRotator);
	SetActorLocation(CoinManageLocation);
	SetCameraOrthographic();
}

void AShopPlayerPawn_FlipSide::MoveSelectCardMode()
{
	SetActorRotation(FRotator::ZeroRotator);
	SetActorLocation(SelectCardLocation);
	SetCameraOrthographic();
}

void AShopPlayerPawn_FlipSide::MoveShopItemMode()
{
	SetActorRotation(FRotator::ZeroRotator);
	SetActorLocation(ShopItemLocation);
	SetCameraOrthographic();
}


void AShopPlayerPawn_FlipSide::MoveShopMainMode()
{
	SetActorLocation(ShopMainLocation);
	SetActorRotation(MainRotation);
	SetCameraPerspective();
}

void AShopPlayerPawn_FlipSide::MoveUnlockWeaponMode()
{
	
	SetActorRotation(FRotator::ZeroRotator);
	SetActorLocation(UnlockWeaponLocation);
	SetCameraPerspective();
}
