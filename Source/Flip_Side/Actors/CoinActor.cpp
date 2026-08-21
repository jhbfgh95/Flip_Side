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

	RefreshCoinMaterial();
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
		StatComponent->OnCCActived.AddDynamic(this, &ACoinActor::OnCCApplied);
		StatComponent->OnCCRemove.AddDynamic(this, &ACoinActor::OnCCRemoved);
		StatComponent->OnStatusEffectsChanged.AddUObject(this, &ACoinActor::HandleStatusEffectsChanged);
		StatComponent->RefreshStatusEffectEvents();
	}
}

void ACoinActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(JumpTimerHandle);
		World->GetTimerManager().ClearTimer(FlashTimerHandle);
	}

	// 진입 연출 도중 제거되어도 ActingSubsystem이 영원히 대기하지 않도록 완료를 보장합니다.
	CompleteLandingCallback();
	if (IsValid(StatComponent))
	{
		StatComponent->OnStatusEffectsChanged.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
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
	RefreshCover();
}

void ACoinActor::SetCoinIsActing(const bool IsActing)
{
	bIsActing = IsActing;
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

	if(StatComponent)
	{
		StatComponent->ApplyFaceWeaponStat(CurrentFace);
	}
}

// BattleManager에서 SetGridPoint 부를 때 X, Y 최대값을 GridManager에서 받아서 그거 넘어가면 Return하고 랜덤값 다시 만드는 코드 있어야함!!
void ACoinActor::SetGridPoint(FGridPoint DecidedGridPoint)
{
	CurrentGridPoint.GridX = DecidedGridPoint.GridX;
	CurrentGridPoint.GridY = DecidedGridPoint.GridY;
}

bool ACoinActor::SetCoinValues(
	int CoinId,
	int FrontId,
	int BackId,
	EWeaponClass WeaponTypes,
	UTexture2D* FrontTexture,
	UTexture2D* BackTexture,
	const FCoinStatInitializeData& StatInitializeData)
{
	if (!IsValid(FrontTexture) || !IsValid(BackTexture) || !IsValid(StatComponent))
	{
		return false;
	}

	CoinID = CoinId;
	FrontWeaponID = FrontId;
	BackWeaponID = BackId;
	WeaponType = WeaponTypes;
	FrontIconTexture = FrontTexture;
	BackIconTexture = BackTexture;
	if (!StatComponent->InitializeCoinStats(StatInitializeData))
	{
		return false;
	}
	RefreshCoinMaterial();
	return true;
}

void ACoinActor::SetCoinOnBattle(const bool IsOnBattle)
{
	bIsOnBattle = IsOnBattle;
}

void ACoinActor::SetUIVisibility(const bool bUIVisibile)
{
	if (IsValid(CoinHPUI))
	{
		CoinHPUI->SetVisibility(bUIVisibile);
	}
}

void ACoinActor::RefreshCoinMaterial()
{
	if (!IsValid(CoinMesh) || !IsValid(FrontIconTexture) || !IsValid(BackIconTexture))
	{
		return;
	}

	UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(CoinMesh->GetMaterial(0));
	if (!IsValid(MID))
	{
		MID = CoinMesh->CreateDynamicMaterialInstance(0);
	}

	if (!IsValid(MID))
	{
		UE_LOG(LogTemp, Error, TEXT("[CoinActor] CoinID=%d 머테리얼 동적 인스턴스 생성에 실패했습니다."), CoinID);
		return;
	}

	static const FName FrontTextureParameter(TEXT("Front_Texture"));
	static const FName BackTextureParameter(TEXT("Back_Texture"));
	static const FName FrontColorParameter(TEXT("Front_Color"));
	static const FName BackColorParameter(TEXT("Back_Color"));
	static const FLinearColor FrontWeaponColor(0.862745f, 0.913725f, 0.313725f, 1.0f);
	static const FLinearColor BackWeaponColor(0.905882f, 0.933333f, 0.917647f, 1.0f);

	MID->SetTextureParameterValue(FrontTextureParameter, FrontIconTexture);
	MID->SetTextureParameterValue(BackTextureParameter, BackIconTexture);
	MID->SetVectorParameterValue(FrontColorParameter, FrontWeaponColor);
	MID->SetVectorParameterValue(BackColorParameter, BackWeaponColor);
}

bool ACoinActor::DoCoinActAtBattleStart(float XLocation, float YLocation, FSimpleDelegate OnLanded)
{
	CompleteLandingCallback();
	PendingLandingDelegate = MoveTemp(OnLanded);
	bLandingCallbackPending = PendingLandingDelegate.IsBound();

	UWorld* World = GetWorld();
	if (!bIsOnBattle || !IsValid(World) || CurrentGridPoint.GridX < 0 || CurrentGridPoint.GridY < 0)
	{
		CompleteLandingCallback();
		return false;
	}

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
	default:
		CompleteLandingCallback();
		return false;
	}

	// 텔포
	SetActorHiddenInGame(false);
	SetActorEnableCollision(false);
	TeleportTo(DecidedGridLocation, FRotator::ZeroRotator);

	if (CoinMesh)
	{
		CoinMesh->SetRelativeRotation(FRotator(AnimStartXRot, 0.f, 0.f));
	}

	// 올라가는 연출
	World->GetTimerManager().ClearTimer(JumpTimerHandle);
	World->GetTimerManager().SetTimer(JumpTimerHandle, this, &ACoinActor::UpdateJump, 0.01f, true);
	return true;
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
		SetUIVisibility(true);
		CompleteLandingCallback();
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

void ACoinActor::CompleteLandingCallback()
{
	if (!bLandingCallbackPending)
	{
		return;
	}

	bLandingCallbackPending = false;
	FSimpleDelegate CompletionDelegate = PendingLandingDelegate;
	PendingLandingDelegate.Unbind();
	CompletionDelegate.ExecuteIfBound();
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
			// TODO: DB 기반 CoinBehaviorPhase 행동 리팩터링이 끝나면 다시 연결합니다.
			// OnClickBattleCoin.Broadcast(this);
		}
	}
}

void ACoinActor::OnRightClicked_Implementation()
{
	if(!GetCoinIsReady() && GetCoinOnBattle() && bIsActing)
	{
		OnCoinRightClicked.Broadcast(this);
	}
}

void ACoinActor::CoinDead()
{
	if (bDeathStarted)
	{
		return;
	}

	bDeathStarted = true;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(JumpTimerHandle);
	}
	CompleteLandingCallback();
	OnCoinDeathStarted.Broadcast(this);

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

void ACoinActor::SetCover(FLinearColor CoverColor, bool bIsShow)
{
	if(CoinActedMesh)
	{
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(CoinActedMesh->GetMaterial(0));
		if(MID)
		{
			MID->SetVectorParameterValue(FName("Coin_Cover_Color"), CoverColor);
		}
		CoinActedMesh->SetVisibility(bIsShow);
	}
}

void ACoinActor::RefreshCover()
{
	if(StatComponent && StatComponent->GetOnIsOnCC())
	{
		if(CoverColors.IsValidIndex(1))
		{
			SetCover(CoverColors[1], true);
		}
		return;
	}

	if(bIsActed)
	{
		if(CoverColors.IsValidIndex(0))
		{
			SetCover(CoverColors[0], true);
		}
		return;
	}

	if(CoverColors.IsValidIndex(0))
	{
		SetCover(CoverColors[0], false);
	}
	else if(CoinActedMesh)
	{
		CoinActedMesh->SetVisibility(false);
	}
}

void ACoinActor::OnCCApplied()
{
	RefreshCover();
}

void ACoinActor::OnCCRemoved()
{
	RefreshCover();
}

void ACoinActor::HandleStatusEffectsChanged(const FStatusEffectsChangedEvent& ChangedEvent)
{
	OnStatusVisualChanged(
		ChangedEvent.BuffTypeID,
		ChangedEvent.SourceType,
		ChangedEvent.SourceDataID,
		ChangedEvent.TotalStackCount,
		ChangedEvent.bIsDebuff,
		ChangedEvent.bIsActive
	);
}
