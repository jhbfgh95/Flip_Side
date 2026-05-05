#include "BossGimmick_RoleTarget.h"
#include "BossActor_RoleTarget.h"
#include "BossManagerSubsystem.h"

void UBossGimmick_RoleTarget::OnPlayerTurnStart(ABossActor* Boss)
{
    if (!Boss) return;

    if (GimmickData.GimmickType != EBossGimmickType::RoleTarget) return;

    ABossActor_RoleTarget* RoleTargetBoss = Cast<ABossActor_RoleTarget>(Boss);
    if (!RoleTargetBoss) return;

    UWorld* World = Boss->GetWorld();
    if (!World) return;

    UBossManagerSubsystem* BossMgr = World->GetSubsystem<UBossManagerSubsystem>();
    if (!BossMgr) return;

    const FBossTurnContext& Ctx = BossMgr->GetTurnContext();
    if (!Ctx.CurrentPattern || !Ctx.CurrentPattern->PatternData.IsValidIndex(Ctx.CurrentPatternIndex)) return;
    if (Ctx.CurrentPattern->PatternData[Ctx.CurrentPatternIndex].GimmickType != EBossGimmickType::RoleTarget) return;

    RoleTargetBoss->StartRoleRoulette();
}

void UBossGimmick_RoleTarget::OnCoinLanded(ABossActor* Boss, FBossTurnContext& Context)
{
    if (!Boss) return;

    if (!Context.CurrentPattern || !Context.CurrentPattern->PatternData.IsValidIndex(Context.CurrentPatternIndex)) return;
    if (Context.CurrentPattern->PatternData[Context.CurrentPatternIndex].GimmickType != EBossGimmickType::RoleTarget) return;

    ABossActor_RoleTarget* RoleTargetBoss = Cast<ABossActor_RoleTarget>(Boss);
    if (!RoleTargetBoss) return;

    UWorld* World = Boss->GetWorld();
    if (!World) return;

    UBossManagerSubsystem* BossMgr = World->GetSubsystem<UBossManagerSubsystem>();
    if (!BossMgr) return;

    RoleTargetBoss->StopRoleRouletteAndLock();
    BossMgr->RecalculateTelegraphForRoleTarget();
}

void UBossGimmick_RoleTarget::OnDamageCalculate(ABossActor* Boss, int32& InOutDamage)
{
    ABossActor_RoleTarget* RoleTargetBoss = Cast<ABossActor_RoleTarget>(Boss);
    if (!RoleTargetBoss || !RoleTargetBoss->IsRoleLocked()) return;

    float Multiplier = 1.0f;
    switch (RoleTargetBoss->GetLockedRoleClass())
    {
        case EWeaponClass::Deal: Multiplier = GimmickData.ParamFloatA > 0.f ? GimmickData.ParamFloatA : 2.5f; break;
        case EWeaponClass::Tank: Multiplier = GimmickData.ParamFloatB > 0.f ? GimmickData.ParamFloatB : 2.0f; break;
        case EWeaponClass::Heal: Multiplier = GimmickData.ParamFloatC > 0.f ? GimmickData.ParamFloatC : 1.5f; break;
        default: break;
    }

    InOutDamage = FMath::RoundToInt(InOutDamage * Multiplier);
    UE_LOG(LogTemp, Warning, TEXT("[RoleTarget] Role=%d, Multiplier=%.2f, Damage=%d"), (int32)RoleTargetBoss->GetLockedRoleClass(), Multiplier, InOutDamage);
}
