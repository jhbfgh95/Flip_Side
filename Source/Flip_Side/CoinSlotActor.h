// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotActor.h"
#include "BattleHoverInterface.h"
#include "BattleClickInterface.h"
#include "AttackAreaTypes.h"
#include "CoinSlotActor.generated.h"

class ACoinActor;

USTRUCT(BlueprintType)
struct FCoinWidgetInfoData
{
	GENERATED_BODY()

	class UTexture2D* Icon;
	FText WeaponName;
	FText RawDescription;
	int32 DefaultBP;
	int32 DefaultAP;
	FAttackAreaSpec AttackAreaSpec;
	FLinearColor TypeColor;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinSlotHovered, ACoinSlotActor*, TargetCoinSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinSlotClicked, ACoinActor*, ReadyTargetCoin );
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoinSlotUnHovered);

UCLASS()
class FLIP_SIDE_API ACoinSlotActor : public ASlotActor, public IBattleHoverInterface, public IBattleClickInterface
{
	GENERATED_BODY()

	ACoinSlotActor();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void OnUnhover_Implementation() override;

	virtual void OnHover_Implementation() override;

	virtual void OnClicked_Implementation() override;

	void SetFrontFaceInfo(
		class UTexture2D* Icon,
		const FText& WeaponName,
		const FText& RawDescription,
		int32 DefaultBP,
		int32 DefaultAP,
		const FAttackAreaSpec& Spec,
		FLinearColor typeColor
	)
	{
		FrontFaceInfo.Icon = Icon;
		FrontFaceInfo.WeaponName = WeaponName;
		FrontFaceInfo.RawDescription = RawDescription;
		FrontFaceInfo.DefaultBP = DefaultBP;
		FrontFaceInfo.DefaultAP = DefaultAP;
		FrontFaceInfo.AttackAreaSpec = Spec;
		FrontFaceInfo.TypeColor = typeColor;
	}

	void SetBackFaceInfo(
		class UTexture2D* Icon,
		const FText& WeaponName,
		const FText& RawDescription,
		int32 DefaultBP,
		int32 DefaultAP,
		const FAttackAreaSpec& Spec,
		FLinearColor typeColor
	)
	{
		BackFaceInfo.Icon = Icon;
		BackFaceInfo.WeaponName = WeaponName;
		BackFaceInfo.RawDescription = RawDescription;
		BackFaceInfo.DefaultBP = DefaultBP;
		BackFaceInfo.DefaultAP = DefaultAP;
		BackFaceInfo.AttackAreaSpec = Spec;
		BackFaceInfo.TypeColor = typeColor;
	}

	UPROPERTY(VisibleAnywhere)
	TArray<ACoinActor*> AllowcatedCoins;
public:
    UPROPERTY(BlueprintAssignable, Category = "Events|Hover")
	FOnCoinSlotHovered OnCoinSlotHovered;

    UPROPERTY(BlueprintAssignable, Category = "Events|Hover")
	FOnCoinSlotUnHovered OnCoinSlotUnHovered;

	UPROPERTY()
	FOnCoinSlotClicked OnCoinSlotClicked;

	UPROPERTY(VisibleAnywhere)
	FCoinWidgetInfoData FrontFaceInfo;

	UPROPERTY(VisibleAnywhere)
	FCoinWidgetInfoData BackFaceInfo;
};
