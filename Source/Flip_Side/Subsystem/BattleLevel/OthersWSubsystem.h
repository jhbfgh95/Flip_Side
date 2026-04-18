#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OthersWSubsystem.generated.h"

class ABase_OtherActor;
class AGridActor;
class UCoinActionManagementWSubsystem;
class UUseableItemWSubsystem;

UCLASS()
class FLIP_SIDE_API UOthersWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<ABase_OtherActor*> ActiveOthers;

	UPROPERTY()
	UCoinActionManagementWSubsystem* CoinActionManager = nullptr;

	UPROPERTY()
	UUseableItemWSubsystem* ItemManager = nullptr;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	void RegisterOther(ABase_OtherActor* TargetOther);

	void UnregisterOther(ABase_OtherActor* TargetOther);

	bool HandleOtherClicked(ABase_OtherActor* TargetOther);

	UFUNCTION()
	bool HandleOtherClickedEvent(ABase_OtherActor* TargetOther);

	UFUNCTION()
	void HandleOtherDead(ABase_OtherActor* TargetOther);

	void RemoveOther(ABase_OtherActor* TargetOther);

	TArray<ABase_OtherActor*> GetActiveOthers() const { return ActiveOthers; }
};
