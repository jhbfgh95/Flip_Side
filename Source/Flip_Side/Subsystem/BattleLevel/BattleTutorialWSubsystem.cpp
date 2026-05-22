// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/BattleLevel/BattleTutorialWSubsystem.h"

#include "Actors/TutorialTargetPoint.h"
#include "BattleTutorialSequenceData.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayerController_FlipSide.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "Subsystem/BattleLevel/BattleManagerWSubsystem.h"
#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "UI/W_BattleTutorialOverlay.h"

namespace
{
	constexpr float BattleTutorialLeverAdvanceDelay = 3.1f;

	FText GetBattleTutorialDisplayText(const FText& SourceText)
	{
		FString DisplayText = SourceText.ToString();

		struct FTextReplacement
		{
			const TCHAR* From;
			const TCHAR* To;
		};

		static const FTextReplacement Replacements[] =
		{
			{
				TEXT("환영합니다. 이번에는 본격적인 게임이라고 할 수 있는 전투에 대해서 설명드리겠습니다."),
				TEXT("환영합니다. 이번 튜토리얼에서는 전투의 기본 목표와 진행 방식을 설명드리겠습니다.")
			},
			{
				TEXT("첫 번째로 화면 구성에 대한 설명입니다."),
				TEXT("첫 번째로 화면 구성에 대해 설명드리겠습니다.")
			},
			{
				TEXT("하이라이트 된 부분은 코인 슬롯입니다."),
				TEXT("하이라이트된 부분은 코인 슬롯입니다.")
			},
			{
				TEXT("코인 슬롯은 상점에서 구매한 코인이 배치되는 곳 입니다."),
				TEXT("코인 슬롯은 전투에 사용할 코인이 배치되는 곳입니다.")
			},
			{
				TEXT("코인 슬롯에는 현재 들어있는 코인에 대한 정보가 나오며, 클릭하여 코인 레디 상태로 바꿀 수 있습니다."),
				TEXT("코인 슬롯에는 현재 들어 있는 코인의 정보가 표시되며, 클릭하여 코인을 레디 상태로 바꿀 수 있습니다.")
			},
			{
				TEXT("클릭해서 코인을 레디 상태로 만들어봅시다."),
				TEXT("클릭해서 코인을 레디 상태로 만들어 봅시다.")
			},
			{
				TEXT("밝은 부분은 레디 상태가 된 코인이 배치되는 서랍입니다."),
				TEXT("밝게 표시된 부분은 레디 상태가 된 코인이 배치되는 서랍입니다.")
			},
			{
				TEXT("레디코인은 10개까지 선택 가능하며, 코인이 사망할 경우 다음 턴에 추가로 코인을 레디시킬 수 있습니다."),
				TEXT("레디 코인은 최대 10개까지 선택할 수 있으며, 코인이 사망할 경우 다음 턴에 추가로 코인을 레디시킬 수 있습니다.")
			},
			{
				TEXT("만약, 전투에서 코인이 생존한다면 생존한 코인들은 자동으로 레디 코인이 됩니다."),
				TEXT("전투에서 생존한 코인은 다음 턴에 자동으로 레디 코인이 됩니다.")
			},
			{
				TEXT("이때 피해를 입은 코인들은 다시 코인 슬롯으로 복귀할 수 없습니다."),
				TEXT("이때 피해를 입은 코인은 다시 코인 슬롯으로 돌아갈 수 없습니다.")
			},
			{
				TEXT("상점에서 구매한 아이템이 배치되는 곳 입니다. 전투 상황에서 아이템을 클릭해 사용할 수 있습니다."),
				TEXT("이곳은 전투 중 사용할 아이템이 배치되는 곳입니다. 전투 상황에서 아이템을 클릭해 사용할 수 있습니다.")
			},
			{
				TEXT("상점에서 구매한 카드가 배치되는 곳 입니다. 카드는 기본적으로 패시브입니다."),
				TEXT("이곳은 전투에 적용되는 카드가 배치되는 곳입니다. 카드는 기본적으로 패시브 효과를 가집니다.")
			},
			{
				TEXT("카드는발동하면 붉은 색으로 불이 들어오게 됩니다."),
				TEXT("카드가 발동하면 붉은색으로 표시됩니다.")
			},
			{
				TEXT("마지막은 게임의 진행에 사용되는 레버입니다."),
				TEXT("마지막은 게임 진행에 사용되는 레버입니다.")
			},
			{
				TEXT("게임 진행은 다음의 순서대로 진행됩니다."),
				TEXT("전투는 다음 순서대로 진행됩니다.")
			},
			{
				TEXT("- 전투에 참가시킬 코인 선택"),
				TEXT("- 전투에 참가시킬 코인을 선택합니다.")
			},
			{
				TEXT("- 레버 클릭 후, 코인이 그리드에 배치되면 행동시킬 코인을 선택합니다."),
				TEXT("- 레버를 클릭하면 코인이 5x8, 총 40개의 그리드로 구성된 필드에 무작위로 뿌려지며 배치됩니다.\n- 배치된 코인들 중 행동시킬 코인을 클릭합니다.")
			},
			{
				TEXT("- 더 이상 행동시키고 싶은 코인이 없으면, 레버를 클릭합니다."),
				TEXT("- 더 이상 행동시키고 싶은 코인이 없으면 레버를 클릭합니다.")
			},
			{
				TEXT("레버를 클릭해서 코인 행동으로 넘어가 봅시다."),
				TEXT("레버를 클릭해서 코인 행동 단계로 넘어가 봅시다.")
			},
			{
				TEXT("이번에는 코인 선택 페이즈 입니다."),
				TEXT("이번에는 코인 선택 페이즈입니다.")
			},
			{
				TEXT("코인을 상황에 따라서 알맞게 행동시키며, 아이템을 적재적소에 사용하며"),
				TEXT("상황에 맞게 코인을 행동시키고, 아이템을 적절히 사용해")
			},
			{
				TEXT("스테이지를 클리어하면, 남은 코인과 아이템은"),
				TEXT("스테이지를 클리어하면 남은 코인과 아이템은")
			},
			{
				TEXT("다음 스테이지의 상점으로 진행하기 전에 정해진 정산 비율에 따라서 환급해줍니다."),
				TEXT("다음 스테이지의 상점으로 진행하기 전에 정해진 정산 비율에 따라 환급됩니다.")
			},
			{
				TEXT("만약, 코인을 전부 소진하게 된다면 게임 오버입니다."),
				TEXT("코인을 전부 소진하면 게임 오버입니다.")
			},
			{
				TEXT("설명이나, UI 구성에서 궁금한 부분이 있으면 오른쪽 위의 ? 버튼을 누르면 됩니다."),
				TEXT("설명이나 UI 구성에서 궁금한 부분이 있으면 오른쪽 위의 ? 버튼을 누르면 됩니다.")
			},
			{
				TEXT("게임을 즐겨보세요!"),
				TEXT("이제 전투를 진행해 봅시다.")
			}
		};

		for (const FTextReplacement& Replacement : Replacements)
		{
			DisplayText.ReplaceInline(Replacement.From, Replacement.To);
		}

		DisplayText.ReplaceInline(
			TEXT("이제 전투를 진행해 봅시다."),
			TEXT("이제 전투를 진행해 봅시다. 전투를 마친 뒤에는 다음 전투를 준비하는 상점을 알아보겠습니다.")
		);

		return FText::FromString(DisplayText);
	}
}

bool UBattleTutorialWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->GetName().Contains(TEXT("L_Stage_Battle_Tutorial"));
}

void UBattleTutorialWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	const float InitDelay = Settings ? Settings->BattleTutorialInitDelay : 0.2f;

	InWorld.GetTimerManager().SetTimer(
		InitBattleTutorialTimerHandle,
		this,
		&UBattleTutorialWSubsystem::InitBattleTutorialFromSettings,
		InitDelay,
		false
	);
}

void UBattleTutorialWSubsystem::InitBattleTutorialFromSettings()
{
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	if (!Settings)
	{
		return;
	}

	UBattleTutorialSequenceData* LoadedSequenceData = Settings->BattleTutorialSequenceData.LoadSynchronous();
	TSubclassOf<UW_BattleTutorialOverlay> LoadedOverlayClass = Settings->BattleTutorialOverlayWidgetClass.LoadSynchronous();

	InitBattleTutorial(LoadedSequenceData, LoadedOverlayClass);
}

void UBattleTutorialWSubsystem::InitBattleTutorial(UBattleTutorialSequenceData* InSequenceData, TSubclassOf<UW_BattleTutorialOverlay> InOverlayClass, int32 ZOrder)
{
	if (!InSequenceData || !InOverlayClass)
	{
		return;
	}

	EndBattleTutorial();

	SequenceData = InSequenceData;
	CurrentStepIndex = 0;
	CurrentStepClickCount = 0;
	bInitialized = true;

	BattlePlayerController = Cast<ABattlePlayerController_FlipSide>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	CoinManager = GetWorld() ? GetWorld()->GetSubsystem<UCoinManagementWSubsystem>() : nullptr;
	BattleManager = GetWorld() ? GetWorld()->GetSubsystem<UBattleManagerWSubsystem>() : nullptr;

	if (BattlePlayerController)
	{
		OverlayWidget = CreateWidget<UW_BattleTutorialOverlay>(BattlePlayerController, InOverlayClass);
	}
	else
	{
		OverlayWidget = CreateWidget<UW_BattleTutorialOverlay>(GetWorld(), InOverlayClass);
	}

	if (!OverlayWidget)
	{
		EndBattleTutorial();
		return;
	}

	OverlayWidget->OnBattleTutorialOverlayClicked.AddUObject(this, &UBattleTutorialWSubsystem::HandleOverlayClicked);
	OverlayWidget->AddToViewport(ZOrder);

	CacheTutorialTargets();
	BindBattleEvents();
	ApplyCurrentStep();
}

void UBattleTutorialWSubsystem::EndBattleTutorial()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InitBattleTutorialTimerHandle);
		World->GetTimerManager().ClearTimer(LeverAdvanceTimerHandle);
	}

	UnbindBattleEvents();

	if (OverlayWidget)
	{
		OverlayWidget->OnBattleTutorialOverlayClicked.RemoveAll(this);
		OverlayWidget->RemoveFromParent();
		OverlayWidget = nullptr;
	}

	SetTutorialInput(false);

	SequenceData = nullptr;
	CoinManager = nullptr;
	BattleManager = nullptr;
	BattlePlayerController = nullptr;
	TutorialTargetMap.Empty();
	CurrentStepIndex = INDEX_NONE;
	CurrentStepClickCount = 0;
	bInitialized = false;
}

void UBattleTutorialWSubsystem::AdvanceBattleTutorial()
{
	if (!bInitialized || !SequenceData)
	{
		return;
	}

	CurrentStepIndex++;
	CurrentStepClickCount = 0;

	if (!SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		EndBattleTutorial();
		return;
	}

	ApplyCurrentStep();
}

void UBattleTutorialWSubsystem::AdvanceAfterLeverAct()
{
	if (OverlayWidget)
	{
		OverlayWidget->SetVisibility(ESlateVisibility::Visible);
	}

	AdvanceBattleTutorial();
}

void UBattleTutorialWSubsystem::CacheTutorialTargets()
{
	TutorialTargetMap.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATutorialTargetPoint::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ATutorialTargetPoint* TargetPoint = Cast<ATutorialTargetPoint>(Actor);
		if (!TargetPoint || TargetPoint->FocusId.IsNone())
		{
			continue;
		}

		TutorialTargetMap.Add(TargetPoint->FocusId, TargetPoint);
	}
}

void UBattleTutorialWSubsystem::BindBattleEvents()
{
	if (CoinManager)
	{
		CoinManager->OnBattleTutorialCoinSlotClicked.AddUObject(this, &UBattleTutorialWSubsystem::HandleCoinSlotClicked);
	}

	if (BattleManager)
	{
		BattleManager->OnBattleTutorialLeverTriggered.AddUObject(this, &UBattleTutorialWSubsystem::HandleLeverTriggered);
	}
}

void UBattleTutorialWSubsystem::UnbindBattleEvents()
{
	if (CoinManager)
	{
		CoinManager->OnBattleTutorialCoinSlotClicked.RemoveAll(this);
	}

	if (BattleManager)
	{
		BattleManager->OnBattleTutorialLeverTriggered.RemoveAll(this);
	}
}

void UBattleTutorialWSubsystem::ApplyCurrentStep()
{
	if (!SequenceData || !OverlayWidget || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		EndBattleTutorial();
		return;
	}

	const FBattleTutorialStep& CurrentStep = SequenceData->Steps[CurrentStepIndex];

	OverlayWidget->SetTutorialText(GetBattleTutorialDisplayText(CurrentStep.Text), CurrentStep.bUseTopTextBox);
	OverlayWidget->SetNextButtonEnabled(CurrentStep.AdvanceType == EBattleTutorialAdvanceType::OverlayClick ||
		CurrentStep.AdvanceType == EBattleTutorialAdvanceType::End);

	SetTutorialInput(CurrentStep.bUIOnly);

	if (CurrentStep.FocusId.IsNone())
	{
		OverlayWidget->SetFocusUV(FVector2D(0.5f, 0.5f), FVector2D::ZeroVector);
	}
	else if (TObjectPtr<ATutorialTargetPoint>* TargetPoint = TutorialTargetMap.Find(CurrentStep.FocusId))
	{
		if (*TargetPoint)
		{
			OverlayWidget->SetFocusFromWorldLocation(BattlePlayerController, (*TargetPoint)->GetActorLocation(), GetHoleSizeForStep(CurrentStep));
		}
	}

	if (CurrentStep.AdvanceType == EBattleTutorialAdvanceType::End)
	{
		// End 단계는 마지막 안내를 보여준 뒤 Overlay 클릭으로 종료한다.
		OverlayWidget->SetNextButtonEnabled(true);
	}
}

void UBattleTutorialWSubsystem::HandleOverlayClicked()
{
	if (!SequenceData || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FBattleTutorialStep& CurrentStep = SequenceData->Steps[CurrentStepIndex];
	if (CurrentStep.AdvanceType == EBattleTutorialAdvanceType::End)
	{
		EndBattleTutorial();
		return;
	}

	if (CurrentStep.AdvanceType != EBattleTutorialAdvanceType::OverlayClick)
	{
		return;
	}

	AdvanceBattleTutorial();
}

void UBattleTutorialWSubsystem::HandleCoinSlotClicked(ACoinActor* ClickedCoin)
{
	if (!SequenceData || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FBattleTutorialStep& CurrentStep = SequenceData->Steps[CurrentStepIndex];
	if (CurrentStep.AdvanceType != EBattleTutorialAdvanceType::CoinSlotClick)
	{
		return;
	}

	CurrentStepClickCount++;
	if (CurrentStepClickCount >= CurrentStep.RequiredClickCount)
	{
		AdvanceBattleTutorial();
	}
}

void UBattleTutorialWSubsystem::HandleLeverTriggered()
{
	if (!SequenceData || !SequenceData->Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FBattleTutorialStep& CurrentStep = SequenceData->Steps[CurrentStepIndex];
	if (CurrentStep.AdvanceType == EBattleTutorialAdvanceType::LeverClick)
	{
		if (OverlayWidget)
		{
			OverlayWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				LeverAdvanceTimerHandle,
				this,
				&UBattleTutorialWSubsystem::AdvanceAfterLeverAct,
				BattleTutorialLeverAdvanceDelay,
				false
			);
		}
		else
		{
			AdvanceAfterLeverAct();
		}
	}
}

void UBattleTutorialWSubsystem::SetTutorialInput(bool bUIOnly)
{
	if (BattlePlayerController)
	{
		BattlePlayerController->SetInputForTutorial(bUIOnly);
	}
}

FVector2D UBattleTutorialWSubsystem::GetHoleSizeForStep(const FBattleTutorialStep& Step) const
{
	if (!Step.HoleSize.IsNearlyZero())
	{
		return Step.HoleSize;
	}

	return GetDefaultHoleSize(Step.FocusId);
}

FVector2D UBattleTutorialWSubsystem::GetDefaultHoleSize(FName FocusId) const
{
	if (FocusId == TEXT("Lever"))
	{
		return FVector2D(0.06f, 0.14f);
	}

	if (FocusId == TEXT("CoinSlot"))
	{
		return FVector2D(0.2f, 0.15f);
	}

	if (FocusId == TEXT("Drawer"))
	{
		return FVector2D(0.3f, 0.25f);
	}

	if (FocusId == TEXT("ItemSlot"))
	{
		return FVector2D(0.08f, 0.1f);
	}

	if (FocusId == TEXT("Card"))
	{
		return FVector2D(0.3f, 1.0f);
	}

	return FVector2D(0.03f, 0.05f);
}
