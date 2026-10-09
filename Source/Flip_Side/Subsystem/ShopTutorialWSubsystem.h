// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ShopTutorialWSubsystem.generated.h"

UENUM(BlueprintType)
enum class EShopTutorialAction : uint8
{
	None,
	DialogueClicked,
	PageOpened,
	
	NavigatorBarOpened,

	PageChangedBoss,
	PageChangedWeapon,
	PageChangedCoin,
	PageChangedCard,
	PageChangedItem,
	PageChangedMain,
	
	WeaponUnlocked,
	WeaponEquipped,
	CardPurchased,
	CardSelected,
	ItemPurchaseClicked,
	ItemPurchased,
	ItemSold,
	CoinWeaponClicked,

	FrontCoinWeaponClicked,
	BackCoinWeaponClicked,

	CoinSideChanged,
	CoinCountIncreased,
	CoinSlotPurchased,

	EndTutorial

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnShopTutorialStepChanged,
	int32,
	NewStepIndex,
	EShopTutorialAction,
	CompletedAction,
	EShopTutorialAction,
	NextAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopTutorialStarted, int32, InitialStepIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopTutorialCompleted);

UCLASS()
class FLIP_SIDE_API UShopTutorialWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	TArray<EShopTutorialAction> TutorialActionSequence;
	int32 CurrentStepIndex = INDEX_NONE;

public:
	// TutorialPresenter가 튜토리얼 순서를 설정한 뒤 호출합니다.
	void SetTutorialActionSequence(const TArray<EShopTutorialAction>& InActionSequence);

	UFUNCTION(BlueprintCallable)
	void StartTutorial();

	// 현재 단계에서 요구하는 행동과 일치할 때만 다음 단계로 진행합니다.
	UFUNCTION(BlueprintCallable)
	bool ReportAction(EShopTutorialAction Action);

	UFUNCTION(BlueprintPure)
	int32 GetCurrentStepIndex() const { return CurrentStepIndex; }

	UFUNCTION(BlueprintPure)
	EShopTutorialAction GetExpectedAction() const;

	UFUNCTION(BlueprintPure)
	bool IsTutorialActive() const;

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialStepChanged OnTutorialStepChanged;

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialStarted OnTutorialStarted;

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialCompleted OnTutorialCompleted;
};
