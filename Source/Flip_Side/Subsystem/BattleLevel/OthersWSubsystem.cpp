#include "Subsystem/BattleLevel/OthersWSubsystem.h"
#include "Base_OtherActor.h"
#include "GridActor.h"
#include "CoinActionManagementWSubsystem.h"
#include "UseableItemWSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UOthersWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CoinActionManager = Collection.InitializeDependency<UCoinActionManagementWSubsystem>();
	ItemManager = Collection.InitializeDependency<UUseableItemWSubsystem>();
}

void UOthersWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if(InWorld.IsGameWorld())
	{
		TArray<AActor*> FoundOthers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABase_OtherActor::StaticClass(), FoundOthers);

		for(AActor* Actor : FoundOthers)
		{
			if(ABase_OtherActor* OtherActor = Cast<ABase_OtherActor>(Actor))
			{
				RegisterOther(OtherActor);
			}
		}
	}
}

bool UOthersWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if(!Super::ShouldCreateSubsystem(Outer)) return false;

	UWorld* World = Cast<UWorld>(Outer);

	if(World)
	{
		FString MapName = World->GetName();
		if(MapName.Contains(TEXT("L_Stage")))
		{
			return true;
		}
	}

	return false;
}

void UOthersWSubsystem::RegisterOther(ABase_OtherActor* TargetOther)
{
	if(!IsValid(TargetOther)) return;

	ActiveOthers.AddUnique(TargetOther);
	TargetOther->OnOtherClicked.BindDynamic(this, &UOthersWSubsystem::HandleOtherClickedEvent);
	TargetOther->OnOtherDead.AddUniqueDynamic(this, &UOthersWSubsystem::HandleOtherDead);
}

void UOthersWSubsystem::UnregisterOther(ABase_OtherActor* TargetOther)
{
	if(!TargetOther) return;

	TargetOther->OnOtherClicked.Unbind();
	TargetOther->OnOtherDead.RemoveAll(this);
	ActiveOthers.Remove(TargetOther);
}

bool UOthersWSubsystem::HandleOtherClicked(ABase_OtherActor* TargetOther)
{
	if(!IsValid(TargetOther)) return false;

	if(CoinActionManager && CoinActionManager->TryExecuteOtherAction(TargetOther))
	{
		return true;
	}

	// 아이템이 Other를 대상으로 삼는 경우가 생기면 여기서 ItemManager로 라우팅.
	return false;
}

bool UOthersWSubsystem::HandleOtherClickedEvent(ABase_OtherActor* TargetOther)
{
	return HandleOtherClicked(TargetOther);
}

void UOthersWSubsystem::HandleOtherDead(ABase_OtherActor* TargetOther)
{
	RemoveOther(TargetOther);
}

void UOthersWSubsystem::RemoveOther(ABase_OtherActor* TargetOther)
{
	if(!IsValid(TargetOther)) return;

	if(AGridActor* OccupiedGrid = TargetOther->GetOccupiedGrid())
	{
		if(OccupiedGrid->GetCurrentOccupied() == TargetOther)
		{
			OccupiedGrid->ClearOccupied();
		}
	}

	UnregisterOther(TargetOther);
	TargetOther->Destroy();
}
