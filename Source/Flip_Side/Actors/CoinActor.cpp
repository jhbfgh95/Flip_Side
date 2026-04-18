#include "CoinActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Component_Status.h"
#include "W_CoinHPWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
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

	CoinActedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Acted Coin Mesh"));
	CoinActedMesh->SetupAttachment(RootComponent);
	CoinActedMesh->SetVisibility(false);

	FracturedCoin = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Fractured Coin"));
	FracturedCoin->SetupAttachment(RootComponent);
	FracturedCoin->SetVisibility(false);
	FracturedCoin->SetSimulatePhysics(false);
	FracturedCoin->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StatComponent = CreateDefaultSubobject<UComponent_Status>(TEXT("StatComponent"));

	CoinHPUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("Coin HP UI"));
	CoinHPUI->SetupAttachment(RootComponent);

	/* 처음 GridPoint는 없는거 (-1) */
	CurrentGridPoint.GridX = -1;
	CurrentGridPoint.GridY = -1;
}

void ACoinActor::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	// TypeColors DB에서 받아오기 / HP도 마찬가지 -> DB만들어주면 이거 고치기
	if (FrontIconTexture && BackIconTexture)
	{
		UMaterialInstanceDynamic *MID = CoinMesh->CreateDynamicMaterialInstance(0);

		if (MID)
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

	if (CoinHPUI)
	{
		HPWidget = Cast<UW_CoinHPWidget>(CoinHPUI->GetUserWidgetObject());

		if (HPWidget && StatComponent)
		{
			StatComponent->OnHpChanged.AddUObject(HPWidget, &UW_CoinHPWidget::ChangeCurrentHp);
			HPWidget->InitHpWidget(StatComponent->GetHP());
		}

		CoinHPUI->SetVisibility(false);
	}

	if(StatComponent)
	{
		StatComponent->OnDead.AddDynamic(this, &ACoinActor::CoinDead);
		StatComponent->OnHpChanged.AddUObject(this, &ACoinActor::OnCoinHpChanged);
	}
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

void ACoinActor::SetCoinIsActed(const bool IsActed)
{ 
	bIsActed = IsActed;
	if(bIsActed)
	{
		CoinActedMesh->SetVisibility(true);
	}
	else
	{
		CoinActedMesh->SetVisibility(false);
	}
	
}

bool ACoinActor::GetCoinIsActed() const
{ 
	return bIsActed; 
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
	if (DecidedFace == EFaceState::None)
		return;

	CurrentFace = DecidedFace;

	if (CurrentFace == EFaceState::Front)
	{
		DecidedWeaponID = FrontWeaponID;
	}
	else if (CurrentFace == EFaceState::Back)
	{
		DecidedWeaponID = BackWeaponID;
	}
}

// BattleManager에서 SetGridPoint 부를 때 X, Y 최대값을 GridManager에서 받아서 그거 넘어가면 Return하고 랜덤값 다시 만드는 코드 있어야함!!
void ACoinActor::SetGridPoint(FGridPoint DecidedGridPoint)
{
	CurrentGridPoint.GridX = DecidedGridPoint.GridX;
	CurrentGridPoint.GridY = DecidedGridPoint.GridY;
}

void ACoinActor::SetCoinValues(int CoinId, int FrontId, int BackId, EWeaponClass WeaponTypes, UTexture2D *FrontTexture, UTexture2D *BackTexture, FLinearColor DecideColor, int32 CoinHP, int32 SlotNum)
{
	if (WeaponTypes != EWeaponClass::None && FrontTexture && BackTexture)
	{
		CoinID = CoinId;
		FrontWeaponID = FrontId;
		BackWeaponID = BackId;
		WeaponType = WeaponTypes;
		FrontIconTexture = FrontTexture;
		BackIconTexture = BackTexture;
		TypeColor = DecideColor;
		StatComponent->SetHP(CoinHP, true);
		SlotIndex = SlotNum;
	}
}

void ACoinActor::SetCoinOnBattle(const bool IsOnBattle)
{
	bIsOnBattle = IsOnBattle;
}

void ACoinActor::SetUIVisibility(const bool bUIVisibile)
{
	CoinHPUI->SetVisibility(bUIVisibile);
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
	if (!bIsOnBattle)
		return;

	if (CurrentGridPoint.GridX == -1 && CurrentGridPoint.GridY == -1)
		return;

	JumpElapsedTime = 0.0f;

	DecidedGridLocation = FVector(XLocation, YLocation, -80.f);
	// 앞뒤
	switch (CurrentFace)
	{
	case EFaceState::Front:
		AnimStartXRot = 1080.0f;
		DecidedCoinRotation = FRotator(0.f, -180.f, 0.f);
		break;
	case EFaceState::Back:
		AnimStartXRot = -1260.0f;
		DecidedCoinRotation = FRotator(-180.f, 0.f, 0.f);
		break;
	}

	// 텔포
	TeleportTo(DecidedGridLocation, FRotator::ZeroRotator);

	if (CoinMesh)
	{
		CoinMesh->SetRelativeRotation(FRotator(AnimStartXRot, 0.f, 0.f));
	}

	// 올라가는 연출
	GetWorld()->GetTimerManager().SetTimer(JumpTimerHandle, this, &ACoinActor::UpdateJump, 0.01f, true);
}

void ACoinActor::UpdateJump()
{
	JumpElapsedTime += 0.01f;
	float Alpha = JumpElapsedTime / JumpDuration;

	if (Alpha >= 1.0f)
	{
		SetActorLocation(DecidedGridLocation);

		if (CoinMesh)
		{
			CoinMesh->SetRelativeRotation(DecidedCoinRotation);

			// 배틀 코인 클릭 가능하도록 콜리전 복구
			SetActorEnableCollision(true);
			CoinMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CoinMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		}

		GetWorld()->GetTimerManager().ClearTimer(JumpTimerHandle);
		return;
	}

	// 포물선 공식
	float ZOffset = 4.0f * JumpHeight * Alpha * (1.0f - Alpha);
	FVector NewLoc = DecidedGridLocation;
	NewLoc.Z += ZOffset;

	float CurrentPitch = FMath::Lerp(AnimStartXRot, DecidedCoinRotation.Pitch, Alpha);

	SetActorLocation(NewLoc);

	if (CoinMesh)
	{
		CoinMesh->SetRelativeRotation(FRotator(CurrentPitch, 0.f, 0.f));
	}
}

void ACoinActor::OnHover_Implementation()
{
	if (GetCoinOnBattle())
	{
		OnHoverBattleCoin.Broadcast(this);
		CoinHoverOutline();
	}
	else
	{
		OnHoverReadyCoin.Broadcast(this);
	}
}

void ACoinActor::OnUnhover_Implementation()
{
	OnUnhoverCoin.Broadcast();
	CoinUnHoverOutline();
}

void ACoinActor::OnClicked_Implementation()
{
	if (GetCoinIsReady() && !GetCoinOnBattle())
	{
		OnClickReadyCoin.Broadcast(this);
	}
	else if (!GetCoinIsReady() && GetCoinOnBattle())
	{
		//아이템 플래그가 켜져서, 아이템을 적용해야하면 아이템 매니저로 델리게이트를 보내고
		if(GetCoinItemFlag())
		{
			//이거 아ㅣㅇ템ㅁ ㅐ니저에 바ㅣㅇㄴ딩.
			OnCoinClickForItemExcute.Broadcast(this);
		}
		else
		{
			//아니면, 일단 배틀상태의 코인이기 때문에 한 번 배틀상태의 코인이라고 말한 후에 막는다.
			// 한 번 Battle상태 들어가서 클릭하면 두 번째 클릭부터는 막음.
			// CoinActionManagementWSubsystem에 바인딩
			if (!GetCoinIsActed())
			{
				OnClickBattleCoin.Broadcast(this);
			}
			else
			{
				return;
			}
		}
	}
}

void ACoinActor::CoinDead()
{
	if (CoinMesh && FracturedCoin)
    {
        CoinMesh->SetVisibility(false);
        CoinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        FracturedCoin->SetVisibility(true);
        FracturedCoin->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        FracturedCoin->SetSimulatePhysics(true);

		FVector CenterLoc = GetActorLocation();
		float Radius = 50.f;
		float Strength = 100.f;

		FracturedCoin->AddRadialImpulse(CenterLoc, Radius, Strength, ERadialImpulseFalloff::RIF_Constant, true);
    }

    if (CoinHPUI)
    {
        CoinHPUI->SetVisibility(false);
    }

	SetLifeSpan(0.6f);
}

void ACoinActor::OnCoinHpChanged(int32 DeltaHP)
{
    if (DeltaHP < 0 && CoinMesh)
    {
        UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(CoinMesh->GetMaterial(0));
        if (MID)
        {
            MID->SetScalarParameterValue(FName("Flash_Intensity"), 2.5f);
            UE_LOG(LogTemp, Warning, TEXT("SDF"));
        }

        // 0.15초 뒤에 ResetFlash 함수를 호출하여 원래 상태로 복구
        GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle, this, &ACoinActor::ResetFlash, 0.15f, false);
    }
}

void ACoinActor::ResetFlash()
{
    if (CoinMesh)
    {
        UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(CoinMesh->GetMaterial(0));
        if (MID)
        {
            MID->SetScalarParameterValue(FName("Flash_Intensity"), 0.0f);
        }
    }
}