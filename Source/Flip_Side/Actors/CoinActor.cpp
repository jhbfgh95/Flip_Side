#include "CoinActor.h"
#include "Engine/GameInstance.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Actors/DebuffComponent.h"
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
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"

ACoinActor::ACoinActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CCOutlineColors.Add(ECCTypes::Blind, FLinearColor(0.6f, 0.2f, 1.0f));
	CCOutlineColors.Add(ECCTypes::Stun, FLinearColor(1.0f, 0.5f, 0.0f));

	CoinRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = CoinRootComp;

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Coin Mesh"));
	CoinMesh->SetupAttachment(RootComponent);

	AttackRangeBracketAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Attack Range Bracket Anchor"));
	AttackRangeBracketAnchor->SetupAttachment(RootComponent);

	AttackRangeBracketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attack Range Bracket Mesh"));
	AttackRangeBracketMesh->SetupAttachment(AttackRangeBracketAnchor);
	AttackRangeBracketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackRangeBracketMesh->SetGenerateOverlapEvents(false);
	AttackRangeBracketMesh->SetCastShadow(false);
	AttackRangeBracketMesh->SetReceivesDecals(false);
	AttackRangeBracketMesh->SetTranslucentSortPriority(102);
	AttackRangeBracketMesh->SetVisibility(false);

	CoinActedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Acted Coin Mesh"));
	CoinActedMesh->SetupAttachment(RootComponent);
	CoinActedMesh->SetVisibility(false);

	FracturedCoin = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Fractured Coin"));
	FracturedCoin->SetupAttachment(RootComponent);
	FracturedCoin->SetVisibility(false);
	FracturedCoin->SetSimulatePhysics(false);
	FracturedCoin->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StatComponent = CreateDefaultSubobject<UComponent_Status>(TEXT("StatComponent"));
	DebuffComponent = CreateDefaultSubobject<UDebuffComponent>(TEXT("DebuffComponent"));
	CCEffectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("CCEffectLocation"));
	CCEffectLocation->SetupAttachment(RootComponent);
	CCDisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CCDisplayMesh"));
	CCDisplayMesh->SetupAttachment(CCEffectLocation);
	CCDisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CCDisplayMesh->SetGenerateOverlapEvents(false);
	CCDisplayMesh->SetCastShadow(false);
	CCDisplayMesh->SetVisibility(false);
	PoisonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PoisonMesh"));
	PoisonMesh->SetupAttachment(RootComponent);
	PoisonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PoisonMesh->SetGenerateOverlapEvents(false);
	PoisonMesh->SetCastShadow(false);
	PoisonMesh->SetVisibility(false);

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
	RefreshPoisonVisual();
	if (IsValid(DebuffComponent))
	{
		DebuffComponent->OnCCChanged.AddUniqueDynamic(this, &ACoinActor::HandleCCVisualChanged);
		HandleCCVisualChanged(DebuffComponent->GetCCType());
	}

	if (IsValid(CoinHPUI))
	{
		CoinHPUI->InitWidget(); // BP의 Heart/Bar 위젯 생성 후 공통 HP 이벤트에 연결합니다.
		HPWidget = Cast<UW_CoinHPWidget>(CoinHPUI->GetUserWidgetObject());

		if (IsValid(HPWidget) && IsValid(StatComponent))
		{
			StatComponent->OnHpChanged.AddUObject(HPWidget, &UW_CoinHPWidget::ChangeCurrentHp);
			StatComponent->OnMaxHPChanged.AddUObject(HPWidget, &UW_CoinHPWidget::ChangeMaxHp);
			StatComponent->OnShieldChanged.AddUObject(HPWidget, &UW_CoinHPWidget::ChangeShield);
			HPWidget->InitializeWithStatus(StatComponent);
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
		StatComponent->OnWeaponStatsChanged.AddUObject(this, &ACoinActor::HandleOutlineStatsChanged);
		StatComponent->RefreshStatusEffectEvents();
	}
	// World에 속한 전투 페이즈만 관찰하며 매니저 로직/수명은 변경하지 않습니다.
	if (UWorld* World = GetWorld())
	{
		if (UBattleManagerWSubsystem* Battle = World->GetSubsystem<UBattleManagerWSubsystem>())
		{
			Battle->OnPhaseChanged.AddUniqueDynamic(this, &ACoinActor::HandleOutlinePhaseChanged);
			HandleOutlinePhaseChanged(Battle->GetCurrentPhase());
		}
	}
}

void ACoinActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bOutlinePhaseActive = false;
	RefreshOutline();
	SetReadySlotHighlighted(false);
	if (UWorld* World = GetWorld())
	{
		if (UBattleManagerWSubsystem* Battle = World->GetSubsystem<UBattleManagerWSubsystem>())
			Battle->OnPhaseChanged.RemoveDynamic(this, &ACoinActor::HandleOutlinePhaseChanged);
		World->GetTimerManager().ClearTimer(JumpTimerHandle);
		World->GetTimerManager().ClearTimer(FlashTimerHandle);
	}

	// 진입 연출 도중 제거되어도 ActingSubsystem이 영원히 대기하지 않도록 완료를 보장합니다.
	CompleteLandingCallback();
	if (IsValid(StatComponent))
	{
		StatComponent->OnStatusEffectsChanged.RemoveAll(this);
		StatComponent->OnWeaponStatsChanged.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ACoinActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Spawn/연출 코드의 Hidden 전환만 가볍게 확인합니다. 스탯 계산은 변경 이벤트에서 합니다.
	if (IsOutlineEligible() != bOutlineWasEligible) RefreshOutline();
}

bool ACoinActor::IsOutlineEligible() const
{
	return bOutlinePhaseActive && bIsOnBattle && !bDeathStarted && !IsHidden() &&
		!bIsActing && !bOutlineJumpActive && !bOutlineHitActive &&
		IsValid(StatComponent) && !StatComponent->IsDead() && CoinID >= 1 && CoinID <= 10;
}

void ACoinActor::HandleOutlinePhaseChanged(EPhaseState Phase)
{
	bOutlinePhaseActive = Phase == EPhaseState::CoinBehaviorPhase;
	if (!bOutlinePhaseActive)
	{
		bFieldOutlineHovered = false;
		bReadySlotHighlighted = false;
	}
	RefreshOutline();
}

void ACoinActor::HandleOutlineStatsChanged(const FWeaponStatsChangedEvent& ChangedEvent)
{
	RefreshOutline();
}

void ACoinActor::RefreshOutline()
{
	bOutlineWasEligible = IsOutlineEligible();
	bOutlineHovered = bOutlineWasEligible && (bFieldOutlineHovered || bReadySlotHighlighted);
	OutlineState = ECoinOutlineState::Hidden;
	FLinearColor Color = NeutralOutlineColor;
	float Thickness = 0.0f;
	if (bOutlineWasEligible)
	{
		const ECCTypes CC = IsValid(DebuffComponent) ? DebuffComponent->GetCCType() : ECCTypes::None;
		if (CC != ECCTypes::None)
		{
			// DebuffComponent가 마지막 CC만 보관하므로 별도 우선순위/중복 저장을 만들지 않습니다.
			OutlineState = ECoinOutlineState::SpecialCC;
			const FLinearColor* CCColor = CCOutlineColors.Find(CC);
			Color = CCColor ? *CCColor : DebuffOutlineColor;
			Thickness = DebuffOutlineThickness;
		}
		else if (bAllMainKeywordsConsumed)
		{
			// 공격만 끝난 시점이나 CC 중단을 전체 메인 키워드 소모와 혼동하지 않습니다.
			OutlineState = ECoinOutlineState::Completed;
			Color = CompletedOutlineColor;
			Thickness = DebuffOutlineThickness;
		}
		else
		{
			const FResolvedWeaponFaceStats Stats = StatComponent->ResolveFaceStats(CurrentFace);
			// RichText와 같은 최종값-기본값을 수치 스탯 3종에 대해 합산합니다. HP/사거리는 제외합니다.
			const int64 Net = int64(Stats.FinalNumericStats.AttackPoint) - Stats.BaseNumericStats.AttackPoint +
				int64(Stats.FinalNumericStats.WeaponPoint) - Stats.BaseNumericStats.WeaponPoint +
				int64(Stats.FinalNumericStats.WeaponCnt) - Stats.BaseNumericStats.WeaponCnt;
			OutlineState = Net > 0 ? ECoinOutlineState::Buff : Net < 0 ? ECoinOutlineState::Debuff : ECoinOutlineState::Neutral;
			Color = Net > 0 ? BuffOutlineColor : Net < 0 ? DebuffOutlineColor : NeutralOutlineColor;
			Thickness = Net < 0 ? DebuffOutlineThickness : BuffOutlineThickness;
		}
		// 호버는 색만 덮습니다. 음수/완료/CC는 호버 중에도 Debuff 두께를 유지합니다.
		if (bOutlineHovered) Color = HoverOutlineColor;
	}
	Thickness = FMath::IsFinite(Thickness) ? FMath::Max(0.0f, Thickness) : 0.0f;
	UWorld* World = GetWorld();
	UMaterialParameterCollectionInstance* Parameters = IsValid(World) && IsValid(OutlineParameterCollection)
		? World->GetParameterCollectionInstance(OutlineParameterCollection) : nullptr;
	bool bParameterWritten = false;
	if (bOutlineWasEligible && IsValid(Parameters))
	{
		// MPC는 공유되므로 코인별 칸을 사용합니다. 다른 코인의 색/두께를 덮어쓰지 않습니다.
		const FName ParameterName(*FString::Printf(TEXT("CoinOutline%d"), CoinID));
		bParameterWritten = Parameters->SetVectorParameterValue(ParameterName, FLinearColor(Color.R, Color.G, Color.B, Thickness));
		if (!bParameterWritten && !bOutlineParameterWarningLogged)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CoinOutline] %s에 Vector Parameter %s가 없습니다."),
				*GetNameSafe(OutlineParameterCollection), *ParameterName.ToString());
			bOutlineParameterWarningLogged = true;
		}
	}
	if (IsValid(CoinMesh))
	{
		// 201~210은 이 코인 프리뷰가 아닌 실제 전투 코인의 전용 Stencil 범위입니다.
		const bool bEnabled = bOutlineWasEligible && bParameterWritten && Thickness > 0.0f;
		CoinMesh->SetCustomDepthStencilValue(bEnabled ? 200 + CoinID : 0);
		CoinMesh->SetRenderCustomDepth(bEnabled);
	}

}

void ACoinActor::SetReadySlotHighlighted(bool bHighlighted)
{
	if (bReadySlotHighlighted == bHighlighted) return;
	bReadySlotHighlighted = bHighlighted;
	// 슬롯 호버와 액터 사망/정리 모두 같은 BP 윤곽선 해제 경로를 사용합니다.
	OnReadySlotHighlightChanged(bHighlighted);
	RefreshOutline();
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
	if (!IsActed) bAllMainKeywordsConsumed = false;
	RefreshCover();
	RefreshOutline();
}

void ACoinActor::MarkAllMainKeywordsConsumed()
{
	bAllMainKeywordsConsumed = true;
	RefreshOutline();
}

bool ACoinActor::ConsumeAdditionalAction()
{
	if (RemainingAdditionalActions <= 0 || !IsValid(StatComponent) || StatComponent->IsDead()) return false;
	--RemainingAdditionalActions;
	SetCoinIsActed(false);
	return true;
}

void ACoinActor::SetCoinIsActing(const bool IsActing)
{
	bIsActing = IsActing;
	RefreshOutline();
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
	RefreshOutline();
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

void ACoinActor::SetWeaponDefinitions(
	const FFaceData& FrontDefinition,
	const FFaceData& BackDefinition)
{
	FrontWeaponDefinition = FrontDefinition;
	BackWeaponDefinition = BackDefinition;
}

const FFaceData* ACoinActor::GetCurrentWeaponDefinition() const
{
	switch (CurrentFace)
	{
	case EFaceState::Front:
		return FrontWeaponDefinition.WeaponID == DecidedWeaponID ? &FrontWeaponDefinition : nullptr;
	case EFaceState::Back:
		return BackWeaponDefinition.WeaponID == DecidedWeaponID ? &BackWeaponDefinition : nullptr;
	default:
		return nullptr;
	}
}

void ACoinActor::SetCoinOnBattle(const bool IsOnBattle)
{
	bIsOnBattle = IsOnBattle;
	RefreshOutline();
}

void ACoinActor::SetUIVisibility(const bool bUIVisibile)
{
	if (IsValid(CoinHPUI))
	{
		CoinHPUI->SetVisibility(bUIVisibile);
	}
}

void ACoinActor::SetAttackRangeBracketVisible(bool bVisible)
{
	if (!IsValid(AttackRangeBracketMesh))
	{
		return;
	}

	const bool bHasConfiguredMesh = IsValid(AttackRangeBracketMesh->GetStaticMesh());
	AttackRangeBracketMesh->SetVisibility(bVisible && bHasConfiguredMesh);
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
	static const FLinearColor FrontWeaponColor(0.32f, 0.19f, 0.035f, 1.0f);
	static const FLinearColor BackWeaponColor(0.16f, 0.20f, 0.25f, 1.0f);

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
	bOutlineJumpActive = true;
	RefreshOutline(); // 등장 및 앞뒤 회전 시작 전에 CustomDepth를 끕니다.
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
	bOutlineJumpActive = false;
	RefreshOutline(); // 콜백이 없는 뒤집기 연출도 착지 시 현재 상태로 복구합니다.
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
	if (GetCoinIsActing()) return;
	if (GetCoinOnBattle())
	{
		OnHoverBattleCoin.Broadcast(this);
		bFieldOutlineHovered = true;
		RefreshOutline(); // 레거시 BP의 단순 On/Off 윤곽선 대신 현재 상태 스타일을 적용합니다.
	}
	else
	{
		OnHoverReadyCoin.Broadcast(this);
	}
}

void ACoinActor::OnUnhover_Implementation()
{
	SetAttackRangeBracketVisible(false);
	OnUnhoverCoin.Broadcast();
	bFieldOutlineHovered = false;
	RefreshOutline();
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
			OnClickBattleCoin.Broadcast(this);
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
	RefreshOutline();
	SetReadySlotHighlighted(false);
	if (IsValid(DebuffComponent)) DebuffComponent->DisableForDeath();
	SetAttackRangeBracketVisible(false);
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
    if (DeltaHP < 0 && IsValid(CoinMesh) && IsValid(GetWorld()) && !bDeathStarted)
    {
		bOutlineHitActive = true;
		RefreshOutline(); // 연속 피격은 기존 타이머를 연장하며 다른 연출의 숨김 상태는 유지합니다.
        UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(CoinMesh->GetMaterial(0));
        if (MID)
        {
            MID->SetScalarParameterValue(FName("Flash_Intensity"), 2.5f);
        }

        // 0.15초 뒤에 ResetFlash 함수를 호출하여 원래 상태로 복구
        GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle, this, &ACoinActor::ResetFlash, 0.15f, false);
    }
}

void ACoinActor::ResetFlash()
{
	bOutlineHitActive = false;
	RefreshOutline();
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
	if(IsValid(StatComponent) && StatComponent->IsStunned())
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

void ACoinActor::HandleCCVisualChanged(ECCTypes CCType)
{
	if (IsValid(CCDisplayMesh))
	{
		// 표시 메쉬는 BP 지정 그대로 유지하고, 상태별 텍스처와 색상만 교체합니다.
		CCDisplayMesh->SetVisibility(false);
		const int32 CCBuffTypeID = CCType == ECCTypes::Blind ? DebuffTypeID::Blind :
			CCType == ECCTypes::Stun ? DebuffTypeID::Stun : INDEX_NONE;
		UGameInstance* GI = GetGameInstance();
		UDataManagerSubsystem* Data = IsValid(GI) ? GI->GetSubsystem<UDataManagerSubsystem>() : nullptr;
		FDebuffDefinitionData Definition;
		if (CCBuffTypeID != INDEX_NONE && IsValid(Data) && Data->TryGetDebuff(CCBuffTypeID, Definition) &&
			IsValid(Definition.Icon) && IsValid(CCDisplayMesh->GetStaticMesh()))
		{
			if (!IsValid(CCDisplayMaterial)) CCDisplayMaterial = CCDisplayMesh->CreateDynamicMaterialInstance(0);
			if (IsValid(CCDisplayMaterial))
			{
				CCDisplayMaterial->SetTextureParameterValue(TEXT("CC_Icon"), Definition.Icon);
				CCDisplayMaterial->SetVectorParameterValue(TEXT("CC_Color"), Definition.Color);
				CCDisplayMesh->SetVisibility(true);
			}
		}
	}
	OnCCVisualChanged(CCType);
	RefreshCover();
	RefreshOutline();
}

void ACoinActor::OnCCRemoved()
{
	RefreshCover();
}

void ACoinActor::HandleStatusEffectsChanged(const FStatusEffectsChangedEvent& ChangedEvent)
{
	RefreshPoisonVisual();
	RefreshOutline();
	OnStatusVisualChanged(
		ChangedEvent.BuffTypeID,
		ChangedEvent.SourceType,
		ChangedEvent.SourceDataID,
		ChangedEvent.TotalStackCount,
		ChangedEvent.bIsDebuff,
		ChangedEvent.bIsActive
	);
}

void ACoinActor::RefreshPoisonVisual()
{
	if (!IsValid(PoisonMesh)) return;
	bool bPoisoned = false;
	if (!bDeathStarted && IsValid(DebuffComponent))
	{
		for (const FStatusEffectInstance& Effect : DebuffComponent->GetDebuffs())
		{
			if (Effect.BuffTypeID == DebuffTypeID::Poison && Effect.RemainingTurns > 0)
			{
				bPoisoned = true;
				break;
			}
		}
	}
	PoisonMesh->SetVisibility(bPoisoned);
}
