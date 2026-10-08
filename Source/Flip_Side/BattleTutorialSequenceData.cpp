// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTutorialSequenceData.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "UI/BattlePlayerHUDWidget.h"
#include "UI/BattleCoinSlotWidget.h"

namespace
{
	void AddWidgetOptions(UWidget* Widget, const FString& Prefix, TArray<FString>& Options, int32 Depth = 0)
	{
		if (!IsValid(Widget) || Depth > 32) return;
		const FString Path = Prefix.IsEmpty() ? Widget->GetName() : Prefix + TEXT("/") + Widget->GetName();
		Options.AddUnique(Path);
		if (UUserWidget* User = Cast<UUserWidget>(Widget))
		{
			if (const UWidgetBlueprintGeneratedClass* Class = Cast<UWidgetBlueprintGeneratedClass>(User->GetClass()))
				if (UWidgetTree* Tree = Class->GetWidgetTreeArchetype())
					AddWidgetOptions(Tree->RootWidget, Path, Options, Depth + 1);
		}
		if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
			for (int32 Index = 0; Index < Panel->GetChildrenCount(); ++Index)
				AddWidgetOptions(Panel->GetChildAt(Index), Path, Options, Depth + 1);
	}
}

TArray<FString> UBattleTutorialSequenceData::GetWidgetHierarchyOptions() const
{
	TArray<FString> Options = {TEXT("")};
	Options.Append({TEXT("@CoinSlotInfo"), TEXT("@CoinSlotInfo/DetailedDescriptionToggleButton"),
		TEXT("@CoinSlotInfo/FrontBookmarkContainer"), TEXT("@CoinSlotInfo/CloseButton"), TEXT("@BattleCoinMobilityBookmark"),
		TEXT("@ItemInfo"), TEXT("@CardInfo")});
	if (const UWidgetBlueprintGeneratedClass* Class = Cast<UWidgetBlueprintGeneratedClass>(TargetHUDClass.Get()))
		if (UWidgetTree* Tree = Class->GetWidgetTreeArchetype()) AddWidgetOptions(Tree->RootWidget, TEXT(""), Options);
	// 런타임에 생성되는 슬롯은 안정적인 슬롯 번호로 지정합니다.
	for (int32 Index = 1; Index <= 10; ++Index)
	{
		const FString Prefix = FString::Printf(TEXT("CoinSlot[%d]"), Index);
		Options.Add(Prefix);
		if (const UBattlePlayerHUDWidget* HUD = TargetHUDClass ? Cast<UBattlePlayerHUDWidget>(TargetHUDClass->GetDefaultObject()) : nullptr)
			if (const auto* Class = Cast<UWidgetBlueprintGeneratedClass>(HUD->GetTutorialCoinSlotClass().Get()))
				if (UWidgetTree* Tree = Class->GetWidgetTreeArchetype()) AddWidgetOptions(Tree->RootWidget, Prefix, Options);
	}
	return Options;
}

TArray<FString> UBattleTutorialSequenceData::GetWidgetHierarchyOptionsForTable()
{
	const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
	UBattleTutorialSequenceData* Sequence = IsValid(Settings) ? Settings->BattleTutorialSequenceData.LoadSynchronous() : nullptr;
	return IsValid(Sequence) ? Sequence->GetWidgetHierarchyOptions() : TArray<FString>{TEXT("")};
}

TArray<FBattleTutorialStep> UBattleTutorialSequenceData::GetOrderedSteps() const
{
	if (!IsValid(StepTable)) return Steps;
	TArray<FBattleTutorialStep> Result;
	if (StepTable->GetRowStruct() != FBattleTutorialStep::StaticStruct())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleTutorial] StepTable must use BattleTutorialStep: %s"), *StepTable->GetPathName());
		return Result;
	}
	TArray<FName> RowNames = StepTable->GetRowNames();
	RowNames.Sort([](const FName& Left, const FName& Right) { return Left.LexicalLess(Right); });
	for (const FName RowName : RowNames)
		if (const FBattleTutorialStep* Row = StepTable->FindRow<FBattleTutorialStep>(RowName, TEXT("BattleTutorial")))
			Result.Add(*Row);
	// Order가 같은 행은 이름 순서를 유지하여 재실행 시에도 같은 순서로 진행합니다.
	Result.StableSort([](const FBattleTutorialStep& Left, const FBattleTutorialStep& Right) { return Left.Order < Right.Order; });
	return Result;
}

#if WITH_EDITOR
bool UBattleTutorialSequenceData::CopyLegacyStepsToTable(UDataTable* InTable)
{
	if (!IsValid(InTable) || InTable->GetRowStruct() != FBattleTutorialStep::StaticStruct() ||
		!InTable->GetRowMap().IsEmpty() || Steps.IsEmpty() || IsValid(StepTable))
		return false;

	for (int32 Index = 0; Index < Steps.Num(); ++Index)
	{
		FBattleTutorialStep Row = Steps[Index];
		Row.Order = Index + 1;
		const FName RowName(*FString::Printf(TEXT("Step_%03d"), Index + 1));
		InTable->AddRow(RowName, Row);
		const FBattleTutorialStep* Copied = InTable->FindRow<FBattleTutorialStep>(RowName, TEXT("BattleTutorialMigration"));
		if (!Copied || !FBattleTutorialStep::StaticStruct()->CompareScriptStruct(&Row, Copied, 0))
		{
			InTable->EmptyTable();
			UE_LOG(LogTemp, Error, TEXT("[BattleTutorial] Could not preserve row %s during DT migration."), *RowName.ToString());
			return false;
		}
	}
	StepTable = InTable;
	InTable->MarkPackageDirty();
	MarkPackageDirty();
	UE_LOG(LogTemp, Display, TEXT("[BattleTutorial] Copied %d legacy steps to %s; all existing fields preserved."), Steps.Num(), *InTable->GetPathName());
	return true;
}
#endif

