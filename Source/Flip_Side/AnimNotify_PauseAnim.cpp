// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PauseAnim.h"
#include "BossActor.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_PauseAnim::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(!MeshComp)
	{
		return;
	}

	if(ABossActor* Boss = Cast<ABossActor>(MeshComp->GetOwner()))
	{
		Boss->FinishBossClearAnimation();
		return;
	}

	MeshComp->bPauseAnims = true;
}
