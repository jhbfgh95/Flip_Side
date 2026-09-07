// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_UnlockWeaponSlot.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UBorder;
class UProgressBar;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedUnlockWeaponSlot, int32, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredUnlockWeaponSlot, int32, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoveredUnlockWeaponSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoldStartedUnlockWeaponSlot, int32, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoldCancelledUnlockWeaponSlot, int32, WeaponID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoldCompletedUnlockWeaponSlot, int32, WeaponID);

UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponName;

	// UMG에서 슬롯 전체를 감싸는 Border의 이름을 HoverBorder로 지정합니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> HoverBorder;

	// UMG에서 길게 누르기 진행 바의 이름을 HoldProgressBar로 지정합니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HoldProgressBar;

protected:
	FFaceData UnlockWeaponData;
	FTimerHandle HoldTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Unlock Weapon|Input")
	float HoldDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Unlock Weapon|Input", meta = (ClampMin = "0.1"))
	float ProgressResetSpeed = 12.0f;

	bool bIsHolding = false;
	bool bIsPointerPressed = false;
	bool bHoldCompleted = false;
	bool bResettingProgress = false;
	float HoldStartTime = 0.0f;

public:
	FOnClickedUnlockWeaponSlot OnClickedUnlockWeaponSlot;
	FOnHoveredUnlockWeaponSlot OnHoveredUnlockWeaponSlot;
	FOnUnhoveredUnlockWeaponSlot OnUnhoveredUnlockWeaponSlot;
	FOnHoldStartedUnlockWeaponSlot OnHoldStartedUnlockWeaponSlot;
	FOnHoldCancelledUnlockWeaponSlot OnHoldCancelledUnlockWeaponSlot;
	FOnHoldCompletedUnlockWeaponSlot OnHoldCompletedUnlockWeaponSlot;

	void InitWidget(const FFaceData& InitWeaponData);
	int32 GetWeaponID() const { return UnlockWeaponData.WeaponID; }

protected:
	UFUNCTION()
	void ClickSlot();
	UFUNCTION()
	void HoverSlot();
	UFUNCTION()
	void UnhoverSlot();
	UFUNCTION()
	void PressSlot();
	UFUNCTION()
	void ReleaseSlot();

	void CompleteHold();
	void CancelHold();
};
