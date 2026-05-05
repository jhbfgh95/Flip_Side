#include "BossGimmick_RoleTarget.h"
#include "BossActor_RoleTarget.h"

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
