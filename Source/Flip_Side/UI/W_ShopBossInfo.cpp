// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopBossInfo.h"
#include "UI/W_ShopBossPatternButton.h"
#include "DataTypes/BossDataTypes.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "Subsystem/BossSetupGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/ShopLevel/ShopBossWSubsystem.h"

void UW_ShopBossInfo::NativeConstruct()
{
	Super::NativeConstruct();

	if(PassiveButton) PassiveButton->OnClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::HandlePassiveButtonClicked);

	RefreshPreparedBossInfo();
}

void UW_ShopBossInfo::RefreshPreparedBossInfo()
{
	UGameInstance* GI = GetGameInstance();
	if(!GI)
	{
		ClearBossInfo();
		return;
	}

	UBossSetupGISubsystem* BossSetupGI = GI->GetSubsystem<UBossSetupGISubsystem>();
	if(!BossSetupGI)
	{
		ClearBossInfo();
		return;
	}

	if(!BossSetupGI->HasPreparedBoss())
	{
		BossSetupGI->PrepareBossForID(1);
	}

	FBossDisplayData PreparedBossData;
	if(!BossSetupGI->GetPreparedBossInfo(PreparedBossData))
	{
		ClearBossInfo();
		return;
	}

	UDataManagerSubsystem* DataMgr = GI->GetSubsystem<UDataManagerSubsystem>();

	const int32 StageIndex = BossSetupGI->GetPreparedBossContext().StageIndex;
	CurrentStatMultiplier = 1.0f;
	CurrentGimmickMultiplier = 1.0f;
	if (DataMgr)
		DataMgr->TryGetStageMultiplier(PreparedBossData.BossID, StageIndex, CurrentStatMultiplier, CurrentGimmickMultiplier);

	TArray<FBossPatternDisplayData> PreparedPatternDataList;
	if(DataMgr)
		DataMgr->TryGetBossPatternDisplay(PreparedBossData.BossID, PreparedPatternDataList);

	SetBossInfo(PreparedBossData, PreparedPatternDataList);
}

void UW_ShopBossInfo::SetBossInfo(const FBossDisplayData& BossData)
{
	TArray<FBossPatternDisplayData> EmptyPatternDataList;
	SetBossInfo(BossData, EmptyPatternDataList);
}

void UW_ShopBossInfo::SetBossInfo(const FBossDisplayData& BossData, const TArray<FBossPatternDisplayData>& PatternDataList)
{
	CurrentBossData = BossData;
	CurrentPatternDataList = PatternDataList;
	CurrentPatternIndex = INDEX_NONE;

	if (!IsValid(PatternPreviewActor))
	{
		if (UWorld* World = GetWorld())
		{
			if (UShopBossWSubsystem* Sub = World->GetSubsystem<UShopBossWSubsystem>())
			{
				PatternPreviewActor = Sub->GetPatternPreviewActor();
			}
		}
	}

	if(IsValid(PatternPreviewActor))
	{
		PatternPreviewActor->ClearPreview();
	}

	SetPatternButtonCount(CurrentPatternDataList.Num());
	RefreshBossTexts();
	RefreshPatternTexts();
	RefreshPatternButtonState();
}

void UW_ShopBossInfo::SelectPattern(int32 PatternIndex)
{
	if(!PatternButtons.IsValidIndex(PatternIndex))
	{
		return;
	}

	if(!CurrentPatternDataList.IsValidIndex(PatternIndex))
	{
		return;
	}

	CurrentPatternIndex = PatternIndex;

	if(IsValid(PatternPreviewActor))
	{
		const FBossPatternDisplayData& PatternData = CurrentPatternDataList[PatternIndex];
		PatternPreviewActor->ShowPatternPreview(PatternData.PatternSpec, PatternData.bNoDamage);
	}

	RefreshPatternTexts();
	RefreshPatternButtonState();
}

void UW_ShopBossInfo::HandlePassiveButtonClicked()
{
	SelectPassive();
	OnPassiveButtonClicked.Broadcast();
}

void UW_ShopBossInfo::HandlePatternButtonClicked(int32 PatternIndex)
{
	SelectPattern(PatternIndex);
	OnPatternButtonClicked.Broadcast(PatternIndex);
}

void UW_ShopBossInfo::SelectPassive()
{
	if (CurrentBossData.BossAbilityDescription.IsEmpty())
		return;

	CurrentPatternIndex = INDEX_NONE;

	if (IsValid(PatternPreviewActor))
		PatternPreviewActor->ClearPreview();

	RefreshPatternTexts();
	RefreshPatternButtonState();
}

void UW_ShopBossInfo::SetPatternButtonCount(int32 PatternCount)
{
	RebuildPatternButtons(PatternCount);

	if(CurrentPatternIndex == INDEX_NONE)
	{
		// Keep the passive selection when only the pattern button count changes.
	}
	else if(PatternCount <= 0)
	{
		CurrentPatternIndex = 0;
	}
	else
	{
		CurrentPatternIndex = FMath::Clamp(CurrentPatternIndex, 0, PatternCount - 1);
	}

	RefreshPatternButtonState();
}

void UW_ShopBossInfo::RebuildPatternButtons(int32 PatternCount)
{
	// PassiveButton도 PatternButtonBox의 자식이므로 전체 ClearChildren은 사용하지 않습니다.
	for (UW_ShopBossPatternButton* PatternButton : PatternButtons)
	{
		if (IsValid(PatternButton))
		{
			PatternButton->RemoveFromParent();
		}
	}
	PatternButtons.Reset();

	if (!PatternButtonBox || !PatternButtonWidgetClass)
		return;

	for (int32 Index = 0; Index < PatternCount; ++Index)
	{
		UW_ShopBossPatternButton* PatternButton = CreateWidget<UW_ShopBossPatternButton>(GetWorld(), PatternButtonWidgetClass);
		if (!IsValid(PatternButton))
			continue;

		FText ButtonLabel;

		if(CurrentPatternDataList[Index].bIsGimmick)
		{
			ButtonLabel = FText::Format(
				NSLOCTEXT("BossInfo", "PatternButtonLabel", "P{0}"),
				FText::AsNumber(Index + 1)
			);
		}
		else
		{
			ButtonLabel = FText::Format(
				NSLOCTEXT("BossInfo", "PatternButtonLabel", "G{0}"),
				FText::AsNumber(Index + 1)
			);
		}
		PatternButton->InitPatternButton(Index,ButtonLabel);
		PatternButton->OnShopBossPatternButtonClicked.AddUniqueDynamic(this, &UW_ShopBossInfo::HandlePatternButtonClicked);
		PatternButtonBox->AddChildToHorizontalBox(PatternButton);
		PatternButtons.Add(PatternButton);
	}
}

void UW_ShopBossInfo::RefreshBossTexts()
{
	if(BossImage)
	{
		if(IsValid(CurrentBossData.BossIcon))
		{
			BossImage->SetBrushFromTexture(CurrentBossData.BossIcon);
			BossImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			BossImage->SetBrushFromTexture(nullptr);
			BossImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(BossNameText)
	{
		BossNameText->SetText(FText::FromString(CurrentBossData.BossName));
	}

	const int32 FinalHP     = FMath::RoundToInt(CurrentBossData.BossHP * CurrentStatMultiplier);
	const int32 FinalShield = FMath::RoundToInt(CurrentBossData.ShieldValue * CurrentStatMultiplier);

	if(BossHPText)
	{
		BossHPText->SetText(FText::Format(NSLOCTEXT("BossInfo", "HPFmt", "HP  {0}"), FText::AsNumber(FinalHP)));
		BossHPText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.2f, 0.2f)));
	}

	if(BossShieldText)
	{
		if (FinalShield > 0)
		{
			BossShieldText->SetText(FText::Format(NSLOCTEXT("BossInfo", "ShieldValFmt", "쉴드  {0}"), FText::AsNumber(FinalShield)));
			BossShieldText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 0.6f, 1.f)));
			BossShieldText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			BossShieldText->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(BossAbilityText)
	{
		BossAbilityText->SetText(CurrentBossData.BossAbilityDescription);
	}

}

void UW_ShopBossInfo::RefreshPatternTexts()
{
	if (CurrentPatternIndex == INDEX_NONE)
	{
		ClearPatternInfo();
		if (PatternRangeImage)
			PatternRangeImage->SetVisibility(ESlateVisibility::Hidden);
		if (PatternIconImage)
		{
			if (IsValid(CurrentBossData.BossPassiveIcon))
			{
				PatternIconImage->SetBrushFromTexture(CurrentBossData.BossPassiveIcon);
				PatternIconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				PatternIconImage->SetBrushFromTexture(nullptr);
				PatternIconImage->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		if (PatternTitleText)
			PatternTitleText->SetText(NSLOCTEXT("BossInfo", "PassiveTitle", "패시브"));
		if (PatternNameText)
		{
			PatternNameText->SetText(NSLOCTEXT("BossInfo", "PassiveName",  "패시브"));
			PatternNameText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.85f, 0.3f)));
		}
		if (PatternDescriptionText)
			PatternDescriptionText->SetText(CurrentBossData.BossAbilityDescription);
		return;
	}

	if(PatternTitleText)
	{
		PatternTitleText->SetText(FText::FromString(TEXT("패턴")));
	}

	if(!CurrentPatternDataList.IsValidIndex(CurrentPatternIndex))
	{
		ClearPatternInfo();
		return;
	}

	const FBossPatternDisplayData& PatternData = CurrentPatternDataList[CurrentPatternIndex];
	const FBossGimmickData* SwampGimmick = CurrentBossData.GimmickList.FindByPredicate([](const FBossGimmickData& Gimmick)
	{
		return Gimmick.GimmickType == EBossGimmickType::GridDebuff;
	});
	const bool bIsSwampPattern = PatternData.GimmickType == EBossGimmickType::GridDebuff && SwampGimmick;
	const bool bHasConditionalDamage = bIsSwampPattern && CurrentPatternIndex == 2;

	// 전투는 DamageRatio를 사용하지 않고, 늪 기믹이 기본 피해를 별도로 결정합니다.
	int32 BaseDamage = PatternData.bNoDamage ? 0 : CurrentBossData.AttackPoint;
	int32 ConditionalBaseDamage = 0;
	if (bIsSwampPattern)
	{
		if (CurrentPatternIndex == 0 || CurrentPatternIndex == 1)
		{
			BaseDamage = PatternData.bNoDamage ? 0 : 1;
		}
		else if (bHasConditionalDamage)
		{
			BaseDamage = FMath::RoundToInt(SwampGimmick->ParamFloatB);
			ConditionalBaseDamage = FMath::RoundToInt(SwampGimmick->ParamFloatC);
		}
	}
	// GetDamageWithPillarBonus와 같은 반올림을 사용하며, 전투 중 누적되는 기둥 보너스는 제외합니다.
	const int32 FinalDamage = static_cast<int32>(FMath::Clamp<int64>(
		FMath::RoundToInt64(static_cast<double>(FMath::Max(0, BaseDamage)) * CurrentStatMultiplier), 0, MAX_int32));
	const int32 ConditionalDamage = static_cast<int32>(FMath::Clamp<int64>(
		FMath::RoundToInt64(static_cast<double>(FMath::Max(0, ConditionalBaseDamage)) * CurrentStatMultiplier), 0, MAX_int32));
	const int32 MaxShield = FMath::Max(0, FMath::RoundToInt(CurrentBossData.ShieldValue * CurrentStatMultiplier));
	const int32 FinalShieldHeal = PatternData.bNoDamage
		? FMath::Clamp(FMath::RoundToInt(PatternData.ShieldHeal * CurrentStatMultiplier), 0, MaxShield) : 0;

	if(PatternRangeImage)
	{
		PatternRangeImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if(PatternIconImage)
	{
		if(IsValid(PatternData.PatternIcon))
		{
			PatternIconImage->SetBrushFromTexture(PatternData.PatternIcon);
			PatternIconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PatternIconImage->SetBrushFromTexture(nullptr);
			PatternIconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(PatternNameText)
	{
		PatternNameText->SetText(FText::FromString(PatternData.PatternName));
		PatternNameText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.85f, 0.3f)));
	}

	if(AttackIconImage)
	{
		AttackIconImage->SetVisibility(FinalDamage > 0 || ConditionalDamage > 0
			? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}

	FText StatText = FinalDamage > 0
		? FText::Format(NSLOCTEXT("BossInfo", "DmgFmt", "공격력  {0}"), FText::AsNumber(FinalDamage))
		: FText::GetEmpty();
	FText DisplayDescription = PatternData.PatternDescription;
	switch (PatternData.GimmickType)
	{
	case EBossGimmickType::Shield:
		if (PatternData.bNoDamage)
		{
			StatText = FText::Format(NSLOCTEXT("BossInfo", "ShieldHealMaxFmt", "쉴드 최대 {0} 회복"), FText::AsNumber(FinalShieldHeal));
			DisplayDescription = FText::Format(NSLOCTEXT("BossInfo", "ShieldHealDescription", "보스가 쉴드를 최대 {0} 회복합니다. 최대 쉴드를 넘겨 회복하지 않습니다."), FText::AsNumber(FinalShieldHeal));
		}
		break;
	case EBossGimmickType::Blind:
		StatText = FText::Format(NSLOCTEXT("BossInfo", "BlindDamageFmt", "피해 {0} · 실명 {1}턴"),
			FText::AsNumber(FinalDamage), FText::AsNumber(PatternData.GimmickParamA));
		DisplayDescription = FText::Format(NSLOCTEXT("BossInfo", "BlindDamageDescription", "범위 내 코인에게 피해 {0}을 주고 {1}턴 동안 실명을 부여합니다.\n<DebuffDesc>•실명 상태에서는 공격이 적중하지 않습니다. 기동 능력은 사용할 수 있습니다.</>"),
			FText::AsNumber(FinalDamage), FText::AsNumber(PatternData.GimmickParamA));
		break;
	case EBossGimmickType::GridDebuff:
		if (bIsSwampPattern && CurrentPatternIndex == 0)
		{
			StatText = FText::Format(NSLOCTEXT("BossInfo", "SwampWeaponPowerFmt", "피해 {0} · {1}턴 무기력 {2}"),
				FText::AsNumber(FinalDamage), FText::AsNumber(PatternData.GimmickParamA), FText::AsNumber(PatternData.GimmickParamB));
			DisplayDescription = FText::Format(NSLOCTEXT("BossInfo", "SwampWeaponPowerDescription", "범위 내 코인에게 피해 {0}을 주고 {1}턴 동안 무기력 {2} 디버프를 부여합니다. 디버프는 중첩되지 않습니다."),
				FText::AsNumber(FinalDamage), FText::AsNumber(PatternData.GimmickParamA), FText::AsNumber(PatternData.GimmickParamB));
		}
		else if (bIsSwampPattern && CurrentPatternIndex == 1)
		{
			StatText = FText::Format(NSLOCTEXT("BossInfo", "SwampAttackPowerFmt", "피해 {0} · {1}턴 공격력 {2}"),
				FText::AsNumber(FinalDamage), FText::AsNumber(PatternData.GimmickParamA), FText::AsNumber(PatternData.GimmickParamB));
			DisplayDescription = FText::Format(NSLOCTEXT("BossInfo", "SwampAttackPowerDescription", "범위 내 코인에게 피해 {0}을 주고 {1}턴 동안 공격력 {2} 디버프를 부여합니다. 디버프는 중첩되지 않습니다."),
				FText::AsNumber(FinalDamage), FText::AsNumber(PatternData.GimmickParamA), FText::AsNumber(PatternData.GimmickParamB));
		}
		else if (bHasConditionalDamage)
		{
			StatText = FText::Format(NSLOCTEXT("BossInfo", "SwampConditionalDamageFmt", "피해 {0} / 디버프 보유 시 {1}"),
				FText::AsNumber(FinalDamage), FText::AsNumber(ConditionalDamage));
			DisplayDescription = FText::Format(NSLOCTEXT("BossInfo", "SwampConditionalDamageDescription", "범위 내 코인에게 피해 {0}을 줍니다. 디버프를 가진 코인에게는 대신 피해 {1}을 줍니다."),
				FText::AsNumber(FinalDamage), FText::AsNumber(ConditionalDamage));
		}
		break;
	case EBossGimmickType::Poison:
		StatText = FText::Format(NSLOCTEXT("BossInfo", "BrandDamageFmt", "피해 {0} · 낙인 {1} × 경과 턴 합계"),
			FText::AsNumber(FinalDamage), FText::AsNumber(PatternData.GimmickParamA));
		DisplayDescription = FText::Format(NSLOCTEXT("BossInfo", "BrandDamageDescription", "범위 내 코인에게 피해 {0}을 주고 낙인을 부여합니다.\n<DebuffDesc>•낙인은 정화하기 전까지 유지됩니다.</>\n<DebuffDesc>•보스 페이즈마다 {1} × 모든 낙인의 경과 턴 합계만큼 피해를 줍니다.</>\n<DebuffDesc>•다시 맞으면 새 낙인이 따로 추가됩니다.</>"),
			FText::AsNumber(FinalDamage), FText::AsNumber(PatternData.GimmickParamA));
		break;
	default:
		break;
	}

	if(PatternAttackText)
	{
		PatternAttackText->SetText(StatText);
	}

	if(PatternDescriptionText)
	{
		PatternDescriptionText->SetText(DisplayDescription);
	}
}

void UW_ShopBossInfo::RefreshPatternButtonState()
{
	if (PassiveButton)
	{
		PassiveButton->SetIsEnabled(CurrentPatternIndex != INDEX_NONE
			&& !CurrentBossData.BossAbilityDescription.IsEmpty());
	}

	for(int32 i = 0; i < PatternButtons.Num(); i++)
	{
		if(!PatternButtons[i]) continue;

		PatternButtons[i]->SetIsEnabled(i != CurrentPatternIndex);
	}
}

void UW_ShopBossInfo::ClearBossInfo()
{
	CurrentBossData = FBossDisplayData{};
	CurrentPatternIndex = 0;
	CurrentPatternDataList.Reset();

	if(BossImage)
	{
		BossImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if(BossNameText)
		BossNameText->SetText(FText::GetEmpty());

	if(BossHPText)
		BossHPText->SetText(FText::GetEmpty());

	if(BossShieldText)
		BossShieldText->SetVisibility(ESlateVisibility::Hidden);

	if(BossAbilityText)
	{
		BossAbilityText->SetText(FText::GetEmpty());
	}

	SetPatternButtonCount(0);
	ClearPatternInfo();
}

void UW_ShopBossInfo::ClearPatternInfo()
{
	if(AttackIconImage)
	{
		AttackIconImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if(PatternRangeImage)
	{
		PatternRangeImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if(PatternIconImage)
	{
		PatternIconImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if(PatternNameText)
	{
		PatternNameText->SetText(FText::GetEmpty());
	}

	if(PatternAttackText)
	{
		PatternAttackText->SetText(FText::GetEmpty());
	}

	if(PatternDescriptionText)
	{
		PatternDescriptionText->SetText(FText::GetEmpty());
	}
}

