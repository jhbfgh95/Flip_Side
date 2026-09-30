#include "AnimNotify_BossApplyPattern.h"
#include "BossActor.h"
#include "BossManagerSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void UAnimNotify_BossApplyPattern::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp)) return;
	UWorld* World = MeshComp->GetWorld();
	if (!IsValid(World) || !World->IsGameWorld()) return;
	UBossManagerSubsystem* Manager = World->GetSubsystem<UBossManagerSubsystem>();
	ABossActor* Boss = Cast<ABossActor>(MeshComp->GetOwner());
	if (!IsValid(Manager) || !IsValid(Boss) || Boss != Manager->GetCurrentBoss() ||
		MeshComp != Boss->BossMesh || !Manager->IsAttackExecuting()) return;
	Manager->ApplyCurrentPattern();
}
