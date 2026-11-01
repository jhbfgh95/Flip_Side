#include "Actors/Others/Turret_OtherActor.h"
#include "BossActor.h"
#include "GridManagerSubsystem.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"

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

void ATurret_OtherActor::OnClicked_Implementation()
{
	if (TryBroadcastOtherClicked() || bIsActed || GetHP() <= 0) return;

	UWorld* World = GetWorld();
	UGridManagerSubsystem* GridManager = IsValid(World) ? World->GetSubsystem<UGridManagerSubsystem>() : nullptr;
	if (!IsValid(GridManager)) return;

	// 호버 캐시가 아닌 클릭 시점의 점유 상태로 실제 공격 대상을 다시 판정합니다.
	TArray<FGridPoint> AttackCells;
	ABossActor* Boss = nullptr;
	GridManager->CollectAttackRangeTargets(TurretSpawnGrid, TurretSpec, AttackCells, Boss);
	// 쓰러지는 중인 보스는 피해를 받지 않으므로, 이때는 쏘지 않고 소리도 내지 않습니다.
	if (IsValid(Boss) && Boss->GetCurrentHP() > 0)
	{
		bIsActed = true;
		// 포탑 공격 소리입니다. 보스 피격 소리는 ApplyDamage 안에서 따로 납니다.
		if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
		{
			SoundManager->PlayTurretFireSound();
		}
		Boss->ApplyDamage(AttackPoint, this);
	}
}
