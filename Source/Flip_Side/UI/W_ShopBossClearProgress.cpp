#include "UI/W_ShopBossClearProgress.h"

#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "UI/W_ShopBossProgressSlot.h"

void UW_ShopBossClearProgress::NativeConstruct()
{
	Super::NativeConstruct();
}

void UW_ShopBossClearProgress::SetBossProgress(const TArray<FBossDisplayData>& InBossDataList, int32 CurrentStage)
{
	if (IsValid(BackgroundButton))
	{
		BackgroundButton->OnClicked.AddUniqueDynamic(this, &UW_ShopBossClearProgress::HandleBackgroundWidget);
		IsCheckNextBoss = false;
		IsPlaiyingOpenAnim = false;
	}

	if (!IsValid(BossProgressBox) || !BossProgressSlotClass)
	{
		IsCheckNextBoss = true;
		return;
	}
	CurrentStageLevel = CurrentStage;
	BossProgressBox->ClearChildren();
	BossProgressBox->SetSlotPadding(FMargin(20.0f));
	ProgressSlots.Reset();

	if (InBossDataList.IsEmpty())
	{
		IsCheckNextBoss = true;
		return;
	}

	if (CurrentStageLevel <= 0)
	{
		IsCheckNextBoss = true;
		return;
	}

	constexpr int32 MaxColumns = 4;
	for (int i =0; i<InBossDataList.Num(); i++)
	{
		UW_ShopBossProgressSlot* ProgressSlot = CreateWidget<UW_ShopBossProgressSlot>(this, BossProgressSlotClass);
		if (!IsValid(ProgressSlot))
		{
			continue;
		}

		const bool bCleared = i+1 < CurrentStage;
		ProgressSlot->InitializeProgressSlot(InBossDataList[i], bCleared);
		ProgressSlot->OnOpenSlotAnimationFinished.AddUniqueDynamic(
			this, &ThisClass::HandleOpenSlotAnimationFinished);
		const int32 Row = i / MaxColumns;
		const int32 Column = i % MaxColumns;
		BossProgressBox->AddChildToUniformGrid(ProgressSlot, Row, Column);
		ProgressSlots.Add(ProgressSlot);
	}
}

void UW_ShopBossClearProgress::HandleOpenSlotAnimationFinished()
{
	IsCheckNextBoss = true;
}

void UW_ShopBossClearProgress::HandleBackgroundWidget()
{
	if(IsCheckNextBoss)
	{
		RemoveFromParent();
	}
	else
	{
		if(CurrentStageLevel == -1 || CurrentStageLevel<-1 ||ProgressSlots.Num()<=CurrentStageLevel)
		{
			return;
		}
		if(!IsPlaiyingOpenAnim)
		{
			IsPlaiyingOpenAnim = true;

			if (CurrentStageLevel <= 0 ||!ProgressSlots.IsValidIndex(CurrentStageLevel -1))
			{
				IsCheckNextBoss = true;
				return;
			}

			ProgressSlots[CurrentStageLevel-1]->OpenSlot();
		}
	}
}
