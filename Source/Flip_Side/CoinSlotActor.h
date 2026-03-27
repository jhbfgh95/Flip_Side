// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotActor.h"
#include "BattleHoverInterface.h"
#include "BattleClickInterface.h"
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
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinSlotHovered, ACoinSlotActor*, TargetCoinSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinSlotClicked, ACoinActor*, ReadyTargetCoin );
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoinSlotUnHovered);

UCLASS()
class FLIP_SIDE_API ACoinSlotActor : public ASlotActor, public IBattleHoverInterface, public IBattleClickInterface
{
	GENERATED_BODY()

	ACoinSlotActor();
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* InvisiblePlane;

	int32 OutCoinNum = 0;
	
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
		int32 DefaultAP
	)
		{
		FrontFaceInfo.Icon = Icon;
		FrontFaceInfo.WeaponName = WeaponName;
		FrontFaceInfo.RawDescription = RawDescription;
		FrontFaceInfo.DefaultBP = DefaultBP;
		FrontFaceInfo.DefaultAP = DefaultAP;
	}

	void SetBackFaceInfo(
		class UTexture2D* Icon,
		const FText& WeaponName, 
		const FText& RawDescription,
		int32 DefaultBP, 
		int32 DefaultAP
	)
	{
		BackFaceInfo.Icon = Icon;
		BackFaceInfo.WeaponName = WeaponName;
		BackFaceInfo.RawDescription = RawDescription;
		BackFaceInfo.DefaultBP = DefaultBP;
		BackFaceInfo.DefaultAP = DefaultAP;
	}

	UPROPERTY(VisibleAnywhere)
	TArray<ACoinActor*> AllowcatedCoins;
public:
	UPROPERTY()
	FOnCoinSlotHovered OnCoinSlotHovered;

	UPROPERTY()
	FOnCoinSlotUnHovered OnCoinSlotUnHovered;

	UPROPERTY()
	FOnCoinSlotClicked OnCoinSlotClicked;

	UPROPERTY(VisibleAnywhere)
	FCoinWidgetInfoData FrontFaceInfo;

	UPROPERTY(VisibleAnywhere)
	FCoinWidgetInfoData BackFaceInfo;
};
