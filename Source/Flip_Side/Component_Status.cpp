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

FActionTask UComponent_Status::GetModifiedStats(int32 WeaponID) 
{
	CalculateFinalStats();

	FActionTask Task;
	Task.WeaponID = WeaponID;
	Task.ModifiedBehaviorPoint = TotalModifiedBehaviorPoint;
	Task.ModifiedAttackPoint = TotalModifiedAttackPoint;
	Task.ModifiedRange = TotalModifiedRange;

	return Task;
}

void UComponent_Status::SetHP(const int32 ApplyHP)
{
	HP = ApplyHP;
	if(HP <= 0)
	{
		
	}
}

void UComponent_Status::SetStatModifier(FBuffStructure& ModifiedStats)
{
	BuffArray.Add(ModifiedStats);
}

void UComponent_Status::CalculateFinalStats()
{
	for(FBuffStructure& Buff : BuffArray)
	{
		TotalModifiedBehaviorPoint += Buff.ModifiedBP;
		TotalModifiedAttackPoint += Buff.ModifiedAP;
		TotalModifiedRange.GridX = Buff.ModifiedRange.GridX;
		TotalModifiedRange.GridY = Buff.ModifiedRange.GridY;
	}
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