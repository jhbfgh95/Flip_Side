#include "BossGimmick_Groggy.h"
#include "BossActor_Groggy.h"

void UBossGimmick_Groggy::OnBattleStart(ABossActor* Boss)
{
    const int32 BaseGroggy = GimmickData.ParamIntA > 0 ? GimmickData.ParamIntA : 20;
    const float Multiplier = Boss ? Boss->GetStageMultiplierGimmick() : 1.0f;
    MaxGroggy = FMath::RoundToInt(BaseGroggy * Multiplier);
    CurrentGroggy = 0;
    bGroggy = false;

    UE_LOG(LogTemp, Warning, TEXT("[Groggy] BattleStart - BaseGroggy=%d, Multiplier=%.2f, MaxGroggy=%d"), BaseGroggy, Multiplier, MaxGroggy);
}

void UBossGimmick_Groggy::OnDamageCalculate(ABossActor* Boss, int32& InOutDamage)
{
    if (bGroggy)
    {
        InOutDamage = FMath::RoundToInt(InOutDamage * 2.0f);
        UE_LOG(LogTemp, Warning, TEXT("[Groggy] Groggy state - Damage x2 = %d"), InOutDamage);
    }
    else
    {
        InOutDamage = FMath::RoundToInt(InOutDamage * 0.2f);
        UE_LOG(LogTemp, Warning, TEXT("[Groggy] Normal state - Damage x0.2 = %d"), InOutDamage);
    }
}

void UBossGimmick_Groggy::AddGroggyValue(int32 Amount, ABossActor* Boss)
{
    if (bGroggy) return;

    CurrentGroggy = FMath::Min(CurrentGroggy + Amount, MaxGroggy);
    UE_LOG(LogTemp, Warning, TEXT("[Groggy] GroggyValue=%d / %d"), CurrentGroggy, MaxGroggy);

    if (ABossActor_Groggy* GroggyBoss = Cast<ABossActor_Groggy>(Boss))
    {
        GroggyBoss->UpdateGroggyBar(CurrentGroggy);
    }

    if (CurrentGroggy >= MaxGroggy)
    {
        bGroggy = true;
        UE_LOG(LogTemp, Warning, TEXT("[Groggy] Groggy state activated!"));
    }
}
