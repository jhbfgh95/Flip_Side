#include "GridActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
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

void AGridActor::SetOccupied(bool IsOccupied, EGridOccupyingType OccupyType)
{
	bIsOccupied = IsOccupied;
	CurrentOccupying= OccupyType;
}

void AGridActor::SetBossAttack(bool bWillAttack)
{
	bIsGonnaAttack = bWillAttack;
	if(TypeColors.IsValidIndex(0))
	{
		UMaterialInstanceDynamic* MID = GridMesh->CreateDynamicMaterialInstance(0);
		if(MID)
		{
			//이따가
			MID->SetVectorParameterValue(FName("Outline_Color"), TypeColors[0]);
		}
	}
}

//그리드 점령되어있는지 (코인, 장애물(추후추가), 소모품)
bool AGridActor::GetIsOccupied() { return bIsOccupied; }

//그리드에 뭐 올라가있는지
EGridOccupyingType AGridActor::GetCurrentOccupyingThing() {  return CurrentOccupying; }

//코인 "순간이동"
FVector AGridActor::GetGridWorldXY()
{
	FTransform GridTransform = GridRootComp->GetComponentTransform();
	
	GridWorldXY = GridTransform.GetLocation();

	return GridWorldXY;
}