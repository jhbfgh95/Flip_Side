// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTutorialSequenceData.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
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

