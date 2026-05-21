#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ShopSoundManagerWSubsystem.generated.h"

UCLASS()
class FLIP_SIDE_API UShopSoundManagerWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	UPROPERTY()
	class UAudioComponent* BGMComponent = nullptr;
};
