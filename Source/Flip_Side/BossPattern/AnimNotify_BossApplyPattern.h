#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_BossApplyPattern.generated.h"

UCLASS()
class FLIP_SIDE_API UAnimNotify_BossApplyPattern : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override { return TEXT("BossApplyPattern"); }
};
