#include "Actors/Others/Turret_OtherActor.h"
#include "BossActor.h"
#include "GridManagerSubsystem.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Subsystem/BattleLevel/CoinActionManagementWSubsystem.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"
#include "TimerManager.h"

namespace
{
	const FName TurretColorParameter(TEXT("Color"));
	const FLinearColor UsedTurretColor = FLinearColor::FromSRGBColor(FColor(0xA4, 0x04, 0x04, 0xFF));
	const TSoftObjectPtr<UNiagaraSystem> TurretAttackVFX(FSoftObjectPath(
		TEXT("/Game/Effect/CoinEffects/NS_DealType_Normal.NS_DealType_Normal")));
	constexpr int32 TurretClickFlashCount = 4;
	constexpr float TurretClickFlashInterval = 0.065f;
}

ATurret_OtherActor::ATurret_OtherActor()
{
	InitialDurationTurns = 1;
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
}

void ATurret_OtherActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(ClickFlashTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void ATurret_OtherActor::InitializeTurret(FGridPoint TargetGrid, const FAttackAreaSpec& AttackAreaSpec)
{
	TurretSpawnGrid = TargetGrid;
	// GridManager가 설치 위치를 원점으로 해석하므로 주입된 Spec의 오프셋도 유지합니다.
	TurretSpec = AttackAreaSpec;
}

void ATurret_OtherActor::SetAttackRangeBracketVisible(bool bVisible)
{
	if (IsValid(AttackRangeBracketMesh))
	{
		AttackRangeBracketMesh->SetVisibility(bVisible);
	}
}

void ATurret_OtherActor::OnHover_Implementation()
{
	// 공격 사거리 표시는 PlayerController의 코인/포탑 공통 경로가 담당합니다.
}

void ATurret_OtherActor::OnUnhover_Implementation()
{
	SetAttackRangeBracketVisible(false);
}

void ATurret_OtherActor::StartClickFlash()
{
	FTimerManager& Timers = GetWorldTimerManager();
	if (Timers.TimerExists(ClickFlashTimer)) return;

	if (!IsValid(TurretMaterial))
	{
		UStaticMeshComponent* Mesh = GetOthersMesh();
		UMaterialInterface* Material = IsValid(Mesh) ? Mesh->GetMaterial(0) : nullptr;
		if (!IsValid(Material) || !Material->GetVectorParameterValue(FMaterialParameterInfo(TurretColorParameter), DefaultTurretColor)) return;
		TurretMaterial = Mesh->CreateDynamicMaterialInstance(0);
		if (!IsValid(TurretMaterial)) return;
	}

	ClickFlashPhase = 0;
	AdvanceClickFlash();
	FTimerManagerTimerParameters Parameters;
	Parameters.bLoop = true;
	Parameters.bMaxOncePerFrame = true;
	Timers.SetTimer(ClickFlashTimer, this, &ATurret_OtherActor::AdvanceClickFlash, TurretClickFlashInterval, Parameters);
}

void ATurret_OtherActor::AdvanceClickFlash()
{
	if (!IsValid(TurretMaterial))
	{
		GetWorldTimerManager().ClearTimer(ClickFlashTimer);
		return;
	}

	const bool bFinished = ClickFlashPhase == TurretClickFlashCount * 2 - 1;
	const bool bFlashWhite = ClickFlashPhase % 2 == 0;
	const FLinearColor RestColor = bFinished && bIsActed ? UsedTurretColor : DefaultTurretColor;
	TurretMaterial->SetVectorParameterValue(TurretColorParameter, bFlashWhite ? FLinearColor::White : RestColor);
	if (bFinished) GetWorldTimerManager().ClearTimer(ClickFlashTimer);
	else ++ClickFlashPhase;
}

void ATurret_OtherActor::OnClicked_Implementation()
{
	if (TryBroadcastOtherClicked() || bIsActed || GetHP() <= 0) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	// 사거리 안에 공격 대상이 없어도 첫 공격 클릭으로 포탑을 사용합니다.
	bIsActed = true;
	StartClickFlash();

	UGridManagerSubsystem* GridManager = World->GetSubsystem<UGridManagerSubsystem>();
	if (!IsValid(GridManager)) return;
	// 호버 캐시가 아닌 클릭 시점의 점유 상태로 실제 공격 대상을 다시 판정합니다.
	TArray<FGridPoint> AttackCells;
	ABossActor* Boss = nullptr;
	GridManager->CollectAttackRangeTargets(TurretSpawnGrid, TurretSpec, AttackCells, Boss);
	// 쓰러지는 중인 보스는 피해를 받지 않으므로, 이때는 쏘지 않고 소리도 내지 않습니다.
	if (IsValid(Boss) && Boss->GetCurrentHP() > 0)
	{
		// 포탑 공격 소리입니다. 보스 피격 소리는 ApplyDamage 안에서 따로 납니다.
		if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
		{
			SoundManager->PlayTurretFireSound();
		}
		if (UNiagaraSystem* Effect = TurretAttackVFX.LoadSynchronous(); IsValid(Effect))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Effect, GetActorLocation());
		}

		const int32 PreviousHP = Boss->GetCurrentHP();
		const int32 PreviousShield = Boss->GetCurrentShield();
		Boss->ApplyDamage(AttackPoint, this);
		if (!IsValid(Boss)) return;

		FWeaponAttackResult AttackResult;
		AttackResult.Boss = Boss;
		AttackResult.HPDamage = FMath::Max(0, PreviousHP - FMath::Max(0, Boss->GetCurrentHP()));
		AttackResult.ShieldDamage = FMath::Max(0, PreviousShield - Boss->GetCurrentShield());
		if (UCoinActionManagementWSubsystem* ActionManager = World->GetSubsystem<UCoinActionManagementWSubsystem>(); IsValid(ActionManager))
		{
			ActionManager->PlayCommonVFX(AttackResult);
		}
	}
}
