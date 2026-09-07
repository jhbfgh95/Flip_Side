#include "ShopPlayerPawn_FlipSide.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"

AShopPlayerPawn_FlipSide::AShopPlayerPawn_FlipSide()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	Camera->AutoPlaneShift = 1.0f;
	Camera->bUpdateOrthoPlanes = false;
}

void AShopPlayerPawn_FlipSide::BeginPlay()
{
	Super::BeginPlay();

	SetActorTransform(ShopMainTransform);

	if (!bIsMoving)
	{
		TargetTransform = ShopMainTransform;
	}
}

void AShopPlayerPawn_FlipSide::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMoving)
	{
		return;
	}

	const FVector NewLocation = FMath::VInterpTo(
		GetActorLocation(), TargetTransform.GetLocation(), DeltaTime, LocationInterpSpeed);
	const FRotator NewRotation = FMath::RInterpTo(
		GetActorRotation(), TargetTransform.Rotator(), DeltaTime, RotationInterpSpeed);

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);

	const bool bLocationCompleted = NewLocation.Equals(TargetTransform.GetLocation(), LocationCompletionTolerance);
	const bool bRotationCompleted = NewRotation.Equals(TargetTransform.Rotator(), RotationCompletionTolerance);
	if (!bLocationCompleted || !bRotationCompleted)
	{
		return;
	}

	SetActorTransform(TargetTransform);
	bIsMoving = false;

	if (bBroadcastOnMoveCompleted)
	{
		bBroadcastOnMoveCompleted = false;
		OnShopPawnMoveCompleted.Broadcast(TargetShopPage);
	}
}

void AShopPlayerPawn_FlipSide::StartMove(const FTransform& NewTargetTransform)
{
	TargetTransform = NewTargetTransform;
	bIsMoving = true;
	bBroadcastOnMoveCompleted = false;
}

void AShopPlayerPawn_FlipSide::StartShopPageMove(
	const FTransform& NewTargetTransform, EShopPage Page)
{
	StartMove(NewTargetTransform);
	TargetShopPage = Page;
	bBroadcastOnMoveCompleted = true;
}

void AShopPlayerPawn_FlipSide::SetCameraOrthographic(float InOrthoWidth)
{
	if(Camera->ProjectionMode != ECameraProjectionMode::Orthographic)
		Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = InOrthoWidth;
}

void AShopPlayerPawn_FlipSide::SetCameraPerspective()
{
	if(Camera->ProjectionMode != ECameraProjectionMode::Perspective)
		Camera->ProjectionMode = ECameraProjectionMode::Perspective;
}

void AShopPlayerPawn_FlipSide::MoveToShopPage(EShopPage Page)
{
	FTransform PageTransform;
	switch (Page)
	{
	case EShopPage::Main:
		SetCameraPerspective();
		PageTransform = ShopMainTransform;
		break;
	case EShopPage::Coin:
		PageTransform = CoinTransform;
		break;
	case EShopPage::Item:
		PageTransform = ShopItemTransform;
		break;
	case EShopPage::Card:
		PageTransform = CardTransform;
		break;
	case EShopPage::UnlockWeapon:
		PageTransform = UnlockWeaponTransform;
		break;
	case EShopPage::Boss:
		PageTransform = CheckBossInfoTransform;
		break;
	case EShopPage::GameStart:
		PageTransform = ShopMainTransform;
		break;
	default:
		return;
	}

	StartShopPageMove(PageTransform, Page);
}

void AShopPlayerPawn_FlipSide::MoveShopMainMode()
{
	StartMove(ShopMainTransform);
	SetCameraPerspective();
}
