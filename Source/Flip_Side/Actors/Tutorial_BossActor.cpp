// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Tutorial_BossActor.h"
#include "W_BossHP.h"

void ATutorial_BossActor::BeginPlay()
{
	Super::BeginPlay();

    BossHpWidget->InitBossShield(MAX_TUTO_BOSS_SHIELD);
}

void ATutorial_BossActor::ApplyDamage(int32 Damage, AActor * DamageCauser)
{
    if(!DamageCauser) return;
    int32 ActualDamageToHP = Damage;

    if(CurrentShield > 0)
    {
        if(CurrentShield - Damage < 0)
        {
            int32 RemainDamage = Damage - CurrentShield; 
            
            BossHpWidget->ChangeCurrentShield(-CurrentShield); 
            CurrentShield = 0;
            
            ActualDamageToHP = RemainDamage;
        }
        else
        {
            BossHpWidget->ChangeCurrentShield(-Damage);
            CurrentShield -= Damage;
            
            if(BossHitAnim) AnimInstance->Montage_Play(BossHitAnim);
            return; 
        }
    }

    if (ActualDamageToHP > 0)
    {
        CurrentHP -= ActualDamageToHP;
        
        BossHpWidget->ChangeCurrentHp(-ActualDamageToHP);

        if(CurrentHP <= 0)
        {
            if (BossHpWidget)
            {
                BossHpWidget->ShowClearImage();
            }

            if(AnimInstance && BossClearAnim)
            {
                AnimInstance->Montage_Play(BossClearAnim);
            }
        }
        else
        {
            if(BossHitAnim) AnimInstance->Montage_Play(BossHitAnim);
        }
    }
	
}

void ATutorial_BossActor::ApplyShieldHeal(int Heal, AActor* HealCauser)
{
    if(!HealCauser) return;
    int32 OldShield = CurrentShield; 
    
    CurrentShield = FMath::Clamp(CurrentShield + Heal, 0, MAX_TUTO_BOSS_SHIELD);

    int32 ActualHealedAmount = CurrentShield - OldShield;

    if (ActualHealedAmount > 0 && BossHpWidget)
    {
        BossHpWidget->ChangeCurrentShield(ActualHealedAmount);
    }
}