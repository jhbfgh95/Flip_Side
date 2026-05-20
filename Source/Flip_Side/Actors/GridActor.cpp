#include "GridActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BattleClickInterface.h"
#include "BattleHoverInterface.h"
#include "FlipSide_Enum.h"
#include "GridTypes.h"
#include "Base_OtherActor.h"
#include "Subsystem/StageCardWSubsystem.h"

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

	if (!bIsPromotionHighlight) return;

	UStageCardWSubsystem* CardSys = GetWorld()->GetSubsystem<UStageCardWSubsystem>();
	if (!CardSys) return;

	CardSys->CheckPromotionOnTick(Cast<ACoinActor>(CurrentObject));
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

		if(ABase_OtherActor* OtherActor = Cast<ABase_OtherActor>(OccupieActor))
		{
			OtherActor->SetOccupiedGrid(this);
		}
	}
}

//그리드 점령되어있는지 (코인, 장애물(추후추가), 소모품)
bool AGridActor::GetIsOccupied() { return bIsOccupied; }

//그리드에 뭐 올라가있는지
EGridOccupyingType AGridActor::GetCurrentOccupyingThing() {  return CurrentOccupying; }

//코인 순간이동 및 장애물등의 설치
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
	ItemFlag = 0;

	bIsCoinRangePreview = false;
	bIsPromotionHighlight = false;
	bIsItemTargetHighlight = false;

	// 늪 상태는 턴 기반으로 별도 관리되므로 여기서 초기화하지 않음
	InitColor();
	CurrentOccupying = EGridOccupyingType::None;
	if(ABase_OtherActor* OtherActor = Cast<ABase_OtherActor>(CurrentObject))
	{
		OtherActor->SetOccupiedGrid(nullptr);
	}
	CurrentObject = nullptr;
}

void AGridActor::ClearOccupiedOnly()
{
	bIsOccupied = false;
	ItemFlag = 0;

	if(ABase_OtherActor* OtherActor = Cast<ABase_OtherActor>(CurrentObject))
	{
		OtherActor->SetOccupiedGrid(nullptr);
	}

	CurrentOccupying = EGridOccupyingType::None;
	CurrentObject = nullptr;
	InitColor();
}

void AGridActor::SetSwamp(int32 RemainingTurns, int32 DebuffAmount, EWeaponClass TargetClass, const FLinearColor& Color)
{
	bHasSwamp = true;
	SwampRemainingTurns = RemainingTurns;
	SwampDebuffAmount = DebuffAmount;
	SwampTargetClass = TargetClass;
	SwampColor = Color;
	SwampColorSet.Color = Color;
	SwampColorSet.Intensity = 0.3f;
	SwampColorSet.DoorOpen = 0.f;
	InitColor();
}

void AGridActor::ClearSwamp()
{
	bHasSwamp = false;
	SwampRemainingTurns = 0;
	SwampDebuffAmount = 0;
	SwampTargetClass = EWeaponClass::None;
	InitColor();
}

void AGridActor::SetSwampPreviewColor(const FLinearColor& Color)
{
	bIsSwampPreview = true;
	SwampPreviewColorSet.Color = Color;
	SwampPreviewColorSet.Intensity = 0.3f;
	SwampPreviewColorSet.DoorOpen = 0.f;

	UMaterialInstanceDynamic* MID = EnsureMID(0);
	if (!MID) return;

	MID->SetVectorParameterValue(TEXT("Cover_Color"), FLinearColor::White);
	MID->SetVectorParameterValue(TEXT("Outline_Color"), SwampPreviewColorSet.Color);
}

void AGridActor::ClearSwampPreviewColor()
{
	bIsSwampPreview = false;

	UMaterialInstanceDynamic* MID = EnsureMID(0);
	if (!MID) return;

	const FLinearColor OutlineColor = bHasSwamp ? SwampColorSet.Color : FLinearColor::White;
	MID->SetVectorParameterValue(TEXT("Outline_Color"), OutlineColor);
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

	MID->SetVectorParameterValue(TEXT("Fill_Color"), OutlineColor);
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

	const bool bShouldShowSwamp = bHasSwamp || bIsSwampPreview;
	const FCachedColorSet& ActiveSwampColorSet = bIsSwampPreview ? SwampPreviewColorSet : SwampColorSet;

	if (bIsCoinRangePreview)
	{
		MID->SetVectorParameterValue(TEXT("Cover_Color"), CoinRangeSet.Color);
		MID->SetVectorParameterValue(TEXT("Fill_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Outline_Color"), FLinearColor::White);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), CoinRangeSet.Intensity);
		MID->SetScalarParameterValue(TEXT("Door_Open"), CoinRangeSet.DoorOpen);
	}
	/*
	else if (bIsPromotionHighlight)
	{
		MID->SetVectorParameterValue(TEXT("Outline_Color"), PromotionColorSet.Color);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), PromotionColorSet.Intensity);
		MID->SetScalarParameterValue(TEXT("Door_Open"), PromotionColorSet.DoorOpen);
	}
	*/
	else if (bIsItemTargetHighlight)
	{
		MID->SetVectorParameterValue(TEXT("Cover_Color"), ItemTargetColorSet.Color);
		MID->SetVectorParameterValue(TEXT("Fill_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Outline_Color"), FLinearColor::White);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), ItemTargetColorSet.Intensity);
		MID->SetScalarParameterValue(TEXT("Door_Open"), ItemTargetColorSet.DoorOpen);
	}
	else if(bIsBossAttack && bShouldShowSwamp)
	{
		MID->SetVectorParameterValue(TEXT("Cover_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Outline_Color"), ActiveSwampColorSet.Color);
		MID->SetVectorParameterValue(TEXT("Fill_Color"), BossColorset.Color);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), BossColorset.Intensity);
		MID->SetScalarParameterValue(TEXT("Door_Open"), BossColorset.DoorOpen);
	}
	else if (bIsBossAttack)
	{
		MID->SetVectorParameterValue(TEXT("Cover_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Outline_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Fill_Color"), BossColorset.Color);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), BossColorset.Intensity);
		MID->SetScalarParameterValue(TEXT("Door_Open"), BossColorset.DoorOpen);
	}
	else if (bShouldShowSwamp)
	{
		MID->SetVectorParameterValue(TEXT("Cover_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Fill_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Outline_Color"), ActiveSwampColorSet.Color);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), ActiveSwampColorSet.Intensity);
		MID->SetScalarParameterValue(TEXT("Door_Open"), ActiveSwampColorSet.DoorOpen);
	}
	else
	{
		MID->SetVectorParameterValue(TEXT("Cover_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Fill_Color"), FLinearColor::White);
		MID->SetVectorParameterValue(TEXT("Outline_Color"), FLinearColor::White);
		MID->SetScalarParameterValue(TEXT("Fill_intensity"), 0.3f);
	}
}

//나이아가라로 변경
void AGridActor::SetPromotionHighlight(bool bOn)
{
	bIsPromotionHighlight = bOn;
	if (bOn)
	{
		PromotionColorSet.Color     = FLinearColor(1.f, 0.84f, 0.f, 1.f);
		PromotionColorSet.Intensity = 0.3f;
		PromotionColorSet.DoorOpen  = 0.f;
	}
	InitColor();
}

void AGridActor::SetItemTargetHighlight(bool bOn)
{
	bIsItemTargetHighlight = bOn;
	if (bOn)
	{
		ItemTargetColorSet.Color = HoverColor.IsValidIndex(2)
			? HoverColor[2]
			: FLinearColor(0.2f, 0.7f, 1.f, 1.f);
		ItemTargetColorSet.Intensity = 1.f;
		ItemTargetColorSet.DoorOpen = 0.f;
	}
	InitColor();
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

	MID->SetVectorParameterValue(TEXT("Cover_Color"),  HoverColor[HoverFlag]);
	MID->SetScalarParameterValue(TEXT("Fill_intensity"), 1.f);
}

void AGridActor::OnUnhover_Implementation()
{
	InitColor();
}
