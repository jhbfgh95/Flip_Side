#include "UI/W_ShopBossProgressSlot.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"

void UW_ShopBossProgressSlot::NativeConstruct()
{
	Super::NativeConstruct();


}

void UW_ShopBossProgressSlot::InitializeProgressSlot(const FBossDisplayData& InBossData, bool bInCleared)
{
	BossData = InBossData;
	bCleared = bInCleared;

	if (IsValid(BossImage))
	{
		BossImage->SetBrushFromTexture(BossData.BossIcon);
		BossImage->SetColorAndOpacity(
			bCleared ? FLinearColor(0.5f, 0.5f, 0.5f, 1.0f) : FLinearColor::White);
	}

	if (IsValid(BossNameText))
	{
		BossNameText->SetText(FText::FromString(BossData.BossName));
	}

	// 이미 클리어한 보스는 등장 연출 없이 열린 모습으로 표시합니다.
	if (bCleared && IsValid(OpenSlotAnimation))
	{
		const float EndTime = OpenSlotAnimation->GetEndTime();
		const float StartTime = FMath::Max(0.0f, EndTime - 0.01f);

		PlayAnimationTimeRange(
			OpenSlotAnimation,
			StartTime,
			EndTime,
			1,
			EUMGSequencePlayMode::Forward,
			1.0f,
			false);

		return;
	}

	if (IsValid(OpenSlotAnimation))
	{
		UnbindAllFromAnimationFinished(OpenSlotAnimation);
		FWidgetAnimationDynamicEvent AnimationFinishedEvent;
		AnimationFinishedEvent.BindDynamic(this, &ThisClass::HandleOpenSlotAnimationFinished);
		BindToAnimationFinished(OpenSlotAnimation, AnimationFinishedEvent);
	}

}

void UW_ShopBossProgressSlot::OpenSlot()
{
	if (IsValid(OpenSlotAnimation))
	{
		PlayAnimation(OpenSlotAnimation);

		// 칸이 실제로 열리는 애니메이션과 함께 소리를 냅니다. 이미 깬 칸은 소리 없이 열린 모습으로 둡니다.
		if (UWorld* World = GetWorld())
		{
			if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
			{
				SoundManager->PlayBossProgressOpenSound();
			}
		}
	}
	else
	{
		OnOpenSlotAnimationFinished.Broadcast();
	}
}

void UW_ShopBossProgressSlot::HandleOpenSlotAnimationFinished()
{
	OnOpenSlotAnimationFinished.Broadcast();
}
