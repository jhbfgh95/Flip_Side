#include "CoinActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Component_Status.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DataTypes/GridTypes.h"
#include "FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"

ACoinActor::ACoinActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CoinRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = CoinRootComp;

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Coin Mesh"));
	CoinMesh->SetupAttachment(RootComponent);
	
	StatComponent = CreateDefaultSubobject<UComponent_Status>(TEXT("StatComponent"));

	CoinHPUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("Coin HP UI"));
	CoinHPUI->SetupAttachment(RootComponent);

	/* 처음 GridPoint는 없는거 (-1) */
	CurrentGridPoint.GridX = -1;
	CurrentGridPoint.GridY = -1;
}

void ACoinActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//TypeColors DB에서 받아오기 / HP도 마찬가지 -> DB만들어주면 이거 고치기
	if(FrontIconTexture && BackIconTexture)
	{
		UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);

		if(MID)
		{
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontIconTexture);
			MID->SetTextureParameterValue(FName("Back_Texture"), BackIconTexture);
			MID->SetVectorParameterValue(FName("Front_Color"), TypeColor);
			MID->SetVectorParameterValue(FName("Back_Color"), TypeColor);
		}
	}
}

void ACoinActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACoinActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ACoinActor::GetSameTypeIndex() const
{
    return SameTypeIndex;
}

int32 ACoinActor::GetFrontWeaponID() const
{
    return FrontWeaponID;
}

void ACoinActor::DecrementSameTypeIndex()
{
    if (SameTypeIndex > 0)
    {
        SameTypeIndex--;
    }
}

void ACoinActor::SetOriginSlotLocation(FVector InLoc)
{
    OriginSlotLocation = InLoc;
}

FVector ACoinActor::GetOriginSlotLocation() const
{
    return OriginSlotLocation;
}

void ACoinActor::SetSameTypeIndex(int32 NewIndex)
{
    SameTypeIndex = NewIndex;
}

void ACoinActor::IncrementSameTypeIndex()
{
    SameTypeIndex++;
}

void ACoinActor::SetCoinIsReady(bool IsReady)
{
	bIsReady = IsReady;
}

bool ACoinActor::GetCoinIsReady() const
{
	return bIsReady;
}

int32 ACoinActor::GetCoinID() const
{
	return CoinID;
}

int32 ACoinActor::GetCoinFaceID() const
{
	return DecidedWeaponID;
}

EFaceState ACoinActor::GetCoinDecidedFace() const
{
	return CurrentFace;
}

FGridPoint ACoinActor::GetDecidedGrid() const
{
	return CurrentGridPoint;
}

void ACoinActor::SetCoinFace(EFaceState DecidedFace)
{
	if(DecidedFace == EFaceState::None) return;

	CurrentFace = DecidedFace;
	
	if(CurrentFace == EFaceState::Front)
	{
		DecidedWeaponID = FrontWeaponID;
	}
	else if(CurrentFace == EFaceState::Back)
	{
		DecidedWeaponID = BackWeaponID;
	}
}

//BattleManager에서 SetGridPoint 부를 때 X, Y 최대값을 GridManager에서 받아서 그거 넘어가면 Return하고 랜덤값 다시 만드는 코드 있어야함!! 
void ACoinActor::SetGridPoint(FGridPoint DecidedGridPoint)
{
	CurrentGridPoint.GridX = DecidedGridPoint.GridX;
	CurrentGridPoint.GridY = DecidedGridPoint.GridY;
}

void ACoinActor::SetCoinValues(int CoinId, int FrontId, int BackId, EWeaponClass WeaponTypes, UTexture2D* FrontTexture, UTexture2D* BackTexture, FLinearColor DecideColor, int32 CoinHP)
{
	if( WeaponTypes != EWeaponClass::None && FrontTexture && BackTexture)
	{
		CoinID = CoinId;
		FrontWeaponID = FrontId;
		BackWeaponID = BackId;
		WeaponType = WeaponTypes;
		FrontIconTexture = FrontTexture;
		BackIconTexture = BackTexture;
		TypeColor = DecideColor;
		StatComponent->SetHP(CoinHP);
	} 

}

void ACoinActor::DoCoinActAtBattleStartLeverDown()
{
	StartX = GetActorLocation().X;
    TargetX = StartX + 1040.f; // 목표: 현재 위치 + 1040
    MoveElapsedTime = 0.0f;

    GetWorld()->GetTimerManager().ClearTimer(LeverMoveTimerHandle);
    
    GetWorld()->GetTimerManager().SetTimer(LeverMoveTimerHandle, this, &ACoinActor::UpdateCoinMoveAtBattleStart, 0.01f, true);
}

void ACoinActor::UpdateCoinMoveAtBattleStart()
{
	MoveElapsedTime += 0.01f;
    
    float Alpha = FMath::Clamp(MoveElapsedTime / MoveTime, 0.0f, 1.0f);

    float NewX = FMath::Lerp(StartX, TargetX, Alpha);

    FVector CurrentLoc = GetActorLocation();
    SetActorLocation(FVector(NewX, CurrentLoc.Y, CurrentLoc.Z));

    if (Alpha >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(LeverMoveTimerHandle);
    }
}

void ACoinActor::DoCoinActAtBattleStart(float XLocation, float YLocation)
{
	if(!bIsReady) return;

	if(CurrentGridPoint.GridX == -1 && CurrentGridPoint.GridY == -1) return;

	JumpElapsedTime = 0.0f;

	DecidedGridLocation = FVector(XLocation, YLocation, -110.f);
	//앞뒤
	switch(CurrentFace)
	{
		case EFaceState::Front:
			AnimStartXRot = 1080.0f;
			DecidedCoinRotation = FRotator(0.f,-180.f,0.f);
			break;
		case EFaceState::Back:
			AnimStartXRot = -1260.0f;
			DecidedCoinRotation = FRotator(-180.f,0.f,0.f);
			break;
	}

	//텔포
	TeleportTo(DecidedGridLocation, FRotator(AnimStartXRot, 0.f, 0.f));

	//올라가는 연출
	GetWorld()->GetTimerManager().SetTimer(JumpTimerHandle, this, &ACoinActor::UpdateJump, 0.01f, true);
}

void ACoinActor::UpdateJump()
{
	JumpElapsedTime += 0.01f;
	float Alpha = JumpElapsedTime / JumpDuration;

	if(Alpha >= 1.0f)
	{
		SetActorLocationAndRotation(DecidedGridLocation, DecidedCoinRotation);
		GetWorld()->GetTimerManager().ClearTimer(JumpTimerHandle);
		return;
	}

	//포물선 공식 (헉!)
	float ZOffset = 4.0f * JumpHeight * Alpha * (1.0f - Alpha);
	FVector NewLoc = DecidedGridLocation;
	NewLoc.Z += ZOffset;

	float CurrentPitch = FMath::Lerp(AnimStartXRot, DecidedCoinRotation.Pitch, Alpha);

	SetActorLocationAndRotation(NewLoc, FRotator(CurrentPitch, 0.f, 0.f));
}