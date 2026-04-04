#include "GridActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BattleClickInterface.h"
#include "BattleHoverInterface.h"
#include "FlipSide_Enum.h"
#include "GridTypes.h"

AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GridRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = GridRootComp;

	GridMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Coin Mesh"));
	GridMesh->SetupAttachment(RootComponent);
}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGridActor::SetGridXY(int32 GridX, int32 GridY)
{
	if(GridX > -1 && GridY > -1)
	{
		GridXY.GridX = GridX;
		GridXY.GridY = GridY;
	}
}

void AGridActor::SetOccupied(bool IsOccupied, EGridOccupyingType OccupyType, AActor* OccupieActor)
{	
	if(OccupieActor)
	{
		bIsOccupied = IsOccupied;
		CurrentOccupying= OccupyType;
		CurrentObject = OccupieActor;
	}
}

//그리드 점령되어있는지 (코인, 장애물(추후추가), 소모품)
bool AGridActor::GetIsOccupied() { return bIsOccupied; }

//그리드에 뭐 올라가있는지
EGridOccupyingType AGridActor::GetCurrentOccupyingThing() {  return CurrentOccupying; }

//코인 "순간이동"
FVector2D AGridActor::GetGridWorldXY()
{
	FTransform GridTransform = GridRootComp->GetComponentTransform();
	
	GridWorldXY = GridTransform.GetLocation();

	return FVector2D(GridWorldXY.X, GridWorldXY.Y);
}

//Destroy는 각 액터들이 HP Stat보고 0되면 this.Destroy로 죽일꺼라서 여기서 굳이 안불러도 상관없다.
void AGridActor::ClearOccupied()
{
	bIsOccupied = false;
	bIsBossAttack = false;
	bBossColorFirstSetted = false;

	bIsCoinRangePreview = false;
	
	InitColor();
	CurrentOccupying = EGridOccupyingType::None;
	CurrentObject = nullptr;
}

AActor* AGridActor::GetCurrentOccupied() const
{
	return CurrentObject;
}

UMaterialInstanceDynamic* AGridActor::EnsureMID(int32 MaterialIndex)
{
	if (!CachedMID)
	{
		if (GridMesh)
		{
			CachedMID = GridMesh->CreateDynamicMaterialInstance(MaterialIndex);
		}
	}
	return CachedMID;
}

void AGridActor::ApplyCellMaterialParams(const FLinearColor& OutlineColor, float FillIntensity, float DoorOpen)
{
	UMaterialInstanceDynamic* MID = EnsureMID(0);
	if (!MID) return;

	MID->SetVectorParameterValue(TEXT("Outline_Color"), OutlineColor);
	MID->SetScalarParameterValue(TEXT("Fill_intensity"), FillIntensity);
	MID->SetScalarParameterValue(TEXT("Door_Open"), DoorOpen);

	if(bIsBossAttack && !bBossColorFirstSetted)
	{
		BossColorset.Color = OutlineColor;
		BossColorset.Intensity = FillIntensity;
		BossColorset.DoorOpen = DoorOpen;
		bBossColorFirstSetted = true;
	}
}

FGridPoint AGridActor::GetGridPoint() const
{
	return GridXY;
}

void AGridActor::InitColor()
{
	UMaterialInstanceDynamic* MID = EnsureMID(0);
	if (!MID) return;

	if (bIsCoinRangePreview)
	{
		MID->SetVectorParameterValue(TEXT("Outline_Color"), CoinRangeSet.Color);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), CoinRangeSet.Intensity);
		MID->SetScalarParameterValue(TEXT("Door_Open"), CoinRangeSet.DoorOpen);
	}
	else if (bIsBossAttack)
	{
		MID->SetVectorParameterValue(TEXT("Outline_Color"), BossColorset.Color);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), BossColorset.Intensity);
		MID->SetScalarParameterValue(TEXT("Door_Open"), BossColorset.DoorOpen);
	}
	else
	{
		MID->SetVectorParameterValue(TEXT("Outline_Color"), FLinearColor(1.f, 1.f, 1.f, 1.f));
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), 0.03f);
	}
}

void AGridActor::OnClicked_Implementation()
{
	if(bIsOccupied) return;
	OnGridClicked.Broadcast(this);
}

void AGridActor::OnHover_Implementation()
{
	UMaterialInstanceDynamic* MID = EnsureMID(0);
	if (!MID) return;

	if(!HoverColor.IsValidIndex(HoverFlag)) return;

	MID->SetVectorParameterValue(TEXT("Outline_Color"),  HoverColor[HoverFlag]);
	MID->SetScalarParameterValue(TEXT("Fill_intensity"), 0.6f);
}

void AGridActor::OnUnhover_Implementation()
{
	InitColor();
}