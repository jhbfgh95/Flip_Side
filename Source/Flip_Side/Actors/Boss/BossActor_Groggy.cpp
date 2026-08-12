#include "BossActor_Groggy.h"
#include "BossGimmick_Groggy.h"

ABossActor_Groggy::ABossActor_Groggy()
{
}

void ABossActor_Groggy::UpdateGroggyBar(int32 CurrentGroggy)
{
    BroadcastBossHUDDataChanged();
}

void ABossActor_Groggy::BeginPlay()
{
    Super::BeginPlay();
}

FBossHUDData ABossActor_Groggy::GetBossHUDData() const
{
    FBossHUDData HUDData = Super::GetBossHUDData();
    HUDData.bUseGroggyAsShield = true;

    for (UBossGimmickBase* Gimmick : GimmickList)
    {
        if (const UBossGimmick_Groggy* GroggyGimmick = Cast<UBossGimmick_Groggy>(Gimmick))
        {
            HUDData.CurrentGroggy = GroggyGimmick->GetCurrentGroggy();
            HUDData.MaxGroggy = GroggyGimmick->GetMaxGroggy();
            break;
        }
    }

    return HUDData;
}

void ABossActor_Groggy::PlayHitAnimation()
{
    if (!BossHitAnim || !AnimInstance)
    {
        return;
    }

    AnimInstance->Montage_Play(BossHitAnim);

    for (UBossGimmickBase* G : GimmickList)
    {
        if (UBossGimmick_Groggy* GroggyGimmick = Cast<UBossGimmick_Groggy>(G))
        {
            const FName SectionName = GroggyGimmick->IsGroggy()
                ? TEXT("GroggyFull")
                : TEXT("GroggyNotFull");

            AnimInstance->Montage_JumpToSection(SectionName, BossHitAnim);
            return;
        }
    }
}

int32 ABossActor_Groggy::ApplyDamageAndReturnHPDamage(int32 Damage, AActor* DamageCauser)
{
    for (UBossGimmickBase* G : GimmickList)
    {
        if (UBossGimmick_Groggy* GroggyGimmick = Cast<UBossGimmick_Groggy>(G))
        {
            GroggyGimmick->OnDamageCalculate(this, Damage);
            break;
        }
    }
    return Super::ApplyDamageAndReturnHPDamage(Damage, DamageCauser);
}

void ABossActor_Groggy::ApplyCC(const FCCStructure& CC)
{
    if (CC.CCType == ECCTypes::None || CC.CCDuration <= 0) return;

    for (UBossGimmickBase* G : GimmickList)
    {
        if (UBossGimmick_Groggy* GroggyGimmick = Cast<UBossGimmick_Groggy>(G))
        {
            GroggyGimmick->AddGroggyValue(CC.CCDuration, this);
            UE_LOG(LogTemp, Warning, TEXT("[BossActor_Groggy] CC absorbed → GroggyValue added"));
            return;
        }
    }

    Super::ApplyCC(CC);
}
