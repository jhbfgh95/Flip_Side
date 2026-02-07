#include "CoinActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
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
	if(FrontIconTexture && BackIconTexture && TypeColors.Num() == 3)
	{
		UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);
		if(MID)
		{
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontIconTexture);
			MID->SetTextureParameterValue(FName("Back_Texture"), BackIconTexture);
			if(WeaponType == EWeaponClass::Tank)
			{
				MID->SetVectorParameterValue(FName("Front_Color"), TypeColors[0]);
				MID->SetVectorParameterValue(FName("Back_Color"), TypeColors[0]);
				HP = 10;
			}
			else if(WeaponType == EWeaponClass::Deal)
			{
				MID->SetVectorParameterValue(FName("Front_Color"), TypeColors[1]);
				MID->SetVectorParameterValue(FName("Back_Color"), TypeColors[1]);
				HP = 5;	
			}
			else if(WeaponType == EWeaponClass::Heal)
			{
				MID->SetVectorParameterValue(FName("Front_Color"), TypeColors[2]);
				MID->SetVectorParameterValue(FName("Back_Color"), TypeColors[2]);
				HP = 5;	
			}
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

int32 ACoinActor::GetCoinID()
{
	return CoinID;
}

EFaceState ACoinActor::GetCoinFace()
{
	return CurrentFace;
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

void ACoinActor::SetCoinValues(int CoinId, int FrontId, int BackId, EWeaponClass WeaponTypes, UTexture2D* FrontTexture, UTexture2D* BackTexture)
{
	if( WeaponTypes != EWeaponClass::None && FrontTexture && BackTexture)
	{
		CoinID = CoinId;
		FrontWeaponID = FrontId;
		BackWeaponID = BackId;
		WeaponType = WeaponTypes;
		FrontIconTexture = FrontTexture;
		BackIconTexture = BackTexture;
	} 

}