#include "UI/W_ShopBossProgressSlot.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

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
		SetAnimationCurrentTime(OpenSlotAnimation, OpenSlotAnimation->GetEndTime());
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
