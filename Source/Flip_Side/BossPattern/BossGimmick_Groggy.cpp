#include "BossGimmick_Groggy.h"
#include "BossActor_Groggy.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"

void UBossGimmick_Groggy::OnBattleStart(ABossActor* Boss)
{
    const int32 BaseGroggy = GimmickData.ParamIntA > 0 ? GimmickData.ParamIntA : 20;
    const float Multiplier = Boss ? Boss->GetStageMultiplierGimmick() : 1.0f;
    MaxGroggy = FMath::RoundToInt(BaseGroggy * Multiplier);
    CurrentGroggy = 0;
    bGroggy = false;

}

void UBossGimmick_Groggy::OnDamageCalculate(ABossActor* Boss, int32& InOutDamage)
{
    if (bGroggy)
    {
        InOutDamage = FMath::RoundToInt(InOutDamage * 2.0f);
    }
    else
    {
        InOutDamage = FMath::RoundToInt(InOutDamage * 0.2f);
    }
}

void UBossGimmick_Groggy::AddGroggyValue(int32 Amount, ABossActor* Boss)
{
    if (bGroggy) return;

    CurrentGroggy = FMath::Min(CurrentGroggy + Amount, MaxGroggy);

    if (ABossActor_Groggy* GroggyBoss = Cast<ABossActor_Groggy>(Boss))
    {
        GroggyBoss->UpdateGroggyBar(CurrentGroggy);
    }

    if (CurrentGroggy >= MaxGroggy)
    {
        bGroggy = true;

        // 게이지가 가득 차 그로기(피해 2배)에 들어가는 순간의 소리입니다.
        UWorld* World = Boss ? Boss->GetWorld() : nullptr;
        if (USoundManagerWSubsystem* SoundManager = World ? World->GetSubsystem<USoundManagerWSubsystem>() : nullptr)
        {
            SoundManager->PlayGroggyBreakSound();
        }
    }
}
