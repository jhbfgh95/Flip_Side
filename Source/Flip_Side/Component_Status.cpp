#include "Component_Status.h"
#include "WeaponDataTypes.h"
#include "CoinDataTypes.h"
#include "GridTypes.h"

UComponent_Status::UComponent_Status()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UComponent_Status::BeginPlay()
{
	Super::BeginPlay();

	OnCCRemove.BindUObject(this, &UComponent_Status::RemoveCC);
}

void UComponent_Status::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

int32 UComponent_Status::GetHP() const
{
	return HP;
}

FActionTask UComponent_Status::GetModifiedStats() 
{
	FActionTask FinalTask;

	FinalTask.ModifiedAttackPoint = 0;
    FinalTask.ModifiedBehaviorPoint = 0;
    FinalTask.ModifiedRange = FGridPoint(0, 0);

	if(OnCalculateStats.IsBound())
	{
		OnCalculateStats.Broadcast(FinalTask);
	}


	return FinalTask;
}

void UComponent_Status::SetHP(const int32 ApplyHP, bool bIsFirst)
{
    if(bIsFirst) 
    {
        MaxHP = ApplyHP;
        HP = ApplyHP;
        return; 
    }
    
    int32 DeltaHP = ApplyHP - HP; 

    HP = FMath::Clamp(ApplyHP, 0, MaxHP);

    OnHpChanged.Broadcast(DeltaHP);
}

void UComponent_Status::ApplyDamage(int32 Damage, AActor* DamageCauser)
{
	int32 FinalDamage = Damage;
	bool bIsIgnored = false;

    if(FinalDamage < 0) FinalDamage = 0;

    if (OnPreTakeDamage.IsBound())
    {
        OnPreTakeDamage.Broadcast(Damage, FinalDamage, bIsIgnored);
    }

    if (bIsIgnored)
    {
        return;
    }

	//독 등의 데미지 뚫고 들어오는 스탯은 나중에 ApplyDamage에 bool로 뚫는지 안뚫는지 flag둬서 하면 됨
	if(Shield > 0)
	{
		Shield = Shield - FinalDamage;
        //0에서 알아서 UI 멈춘다고 가정
        OnShieldChanged.Broadcast(FinalDamage * (-1));
        if(Shield < 0)
        {
            //이거 Shield값 사실상 음수임
            HPChanged(Shield * (-1));
            Shield = 0;
        }
	}
	else
	{
		HPChanged(Damage);
	}

    if (HP <= 0)
    {
        if (OnDead.IsBound())
        {
            OnDead.Broadcast();
        }
    }
}

void UComponent_Status::HPChanged(int32 Damage)
{
    HP = FMath::Clamp(HP - Damage, 0, 9999);
    OnHpChanged.Broadcast(Damage * (-1));
    if (HP <= 0)
    {
        if (OnDead.IsBound())
        {
            OnDead.Broadcast();
            return;
        }
    }
}

void UComponent_Status::AddBuffs(const FBuffInfo&Info)
{
    FBuffInfo NewBuff = Info;

    if (Info.StatDelegate.IsBound())
    {
        NewBuff.StatHandle = OnCalculateStats.Add(Info.StatDelegate);
    }

    if (Info.DamageDelegate.IsBound())
    {
        NewBuff.DamageHandle = OnPreTakeDamage.Add(Info.DamageDelegate);
    }

    if (Info.PreGiveDelegate.IsBound())
    {
        NewBuff.PreGiveHandle = OnPreGiveDamage.Add(Info.PreGiveDelegate);
    }

    if (Info.PostGiveDelegate.IsBound())
    {
        NewBuff.PostGiveHandle = OnPostGiveDamage.Add(Info.PostGiveDelegate);
    }

    ActiveBuffs.Add(NewBuff);

    OnBuffListChanged.Broadcast(true);
}

void UComponent_Status::CheckAttackerPreBuff(AActor* Target, int32 InDmg, int32& OutDmg)
{
    if (OnPreGiveDamage.IsBound())
    {
        OnPreGiveDamage.Broadcast(Target, InDmg, OutDmg);
    }
}

void UComponent_Status::CheckAttackerPostBuff(AActor* Target, int32 DealtDmg)
{
    if (OnPostGiveDamage.IsBound())
    {
        OnPostGiveDamage.Broadcast(Target, DealtDmg);
    }
}

void UComponent_Status::ClearTurnBasedBuffs()
{
	for(const FBuffInfo& Buff : ActiveBuffs)
    {
        if(Buff.StatHandle.IsValid()) OnCalculateStats.Remove(Buff.StatHandle);
        if(Buff.DamageHandle.IsValid()) OnPreTakeDamage.Remove(Buff.DamageHandle);
		if(Buff.PreGiveHandle.IsValid()) OnPreGiveDamage.Remove(Buff.PreGiveHandle);
        if(Buff.PostGiveHandle.IsValid()) OnPostGiveDamage.Remove(Buff.PostGiveHandle);
    }
	
	ActiveBuffs.Empty();
	
	if(OnBuffListChanged.IsBound())
	{
		OnBuffListChanged.Broadcast(false);
	}
}

void UComponent_Status::ApplyHeal(int32 Heal, AActor* HealCauser)
{
	if(HP <= 0) return;
	
	HP = FMath::Clamp(HP + Heal, 0, MaxHP);

	OnHpChanged.Broadcast(Heal);
}

void UComponent_Status::ApplyShield(int32 AddShield, AActor* ShieldCauser)
{
	if(HP <= 0) return;
	Shield = FMath::Clamp(Shield + AddShield, 0, MAX_SHIELD);
}

void UComponent_Status::ApplyCC(FCCStructure CC)
{
	AppliedCC = CC;
	bIsOnCC = true;

	CCDuration = AppliedCC.CCDuration;
}

void UComponent_Status::RemoveCC()
{
	AppliedCC = FCCStructure();

	CCDuration = 0;
	bIsOnCC = false;
}