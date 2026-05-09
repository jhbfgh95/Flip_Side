#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ShopBossWSubsystem.generated.h"

class ABossPatternPreviewActor;

UCLASS()
class FLIP_SIDE_API UShopBossWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	void RegisterPatternPreviewActor(ABossPatternPreviewActor* Actor) { PatternPreviewActor = Actor; }
	ABossPatternPreviewActor* GetPatternPreviewActor() const { return PatternPreviewActor; }

private:
	UPROPERTY()
	TObjectPtr<ABossPatternPreviewActor> PatternPreviewActor = nullptr;
};
