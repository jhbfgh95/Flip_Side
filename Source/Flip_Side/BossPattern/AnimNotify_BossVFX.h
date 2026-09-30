#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BossDataTypes.h"
#include "AnimNotify_BossVFX.generated.h"

UCLASS()
class FLIP_SIDE_API UAnimNotify_BossVFX : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 재생할 Niagara 이펙트 (미설정 시 패턴 데이터의 PatternEffect 사용)
	UPROPERTY(EditAnywhere, Category = "Boss VFX")
	TSoftObjectPtr<class UNiagaraSystem> OverrideEffect;

	UPROPERTY(EditAnywhere, Category = "Boss VFX")
	bool bOverrideEffectTarget = false;

	// 기본값은 패턴 DB의 타겟을 사용하며, 체크 시 아래 설정을 사용합니다.
	UPROPERTY(EditAnywhere, Category = "Boss VFX")
	EBossPatternTarget EffectTarget = EBossPatternTarget::TargetCells;

	// 미설정(ZeroVector) 시 패턴 데이터의 PatternScale 사용
	UPROPERTY(EditAnywhere, Category = "Boss VFX")
	FVector ScaleOverride = FVector::ZeroVector;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override { return TEXT("BossVFX"); }
};
