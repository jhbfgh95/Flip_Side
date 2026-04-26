// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_BossHP.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_BossHP : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "HP")
	int32 MaxHp = 0;
	UPROPERTY(VisibleAnywhere,  Category = "HP")
	int32 CurrentHp = 0;

	UPROPERTY(VisibleAnywhere,  Category = "Shield")
	int32 MaxShield = 0;
	UPROPERTY(VisibleAnywhere,  Category = "Shield")
	int32 CurrentShield = 0;

	UPROPERTY(EditAnywhere, Category = "BossHP|Animation", meta = (AllowPrivateAccess))
	float ProgressBarInterpSpeed = 5.f;

	float DisplayHpPercent = 0.f;
	float TargetHpPercent = 0.f;
	float DisplayShieldPercent = 0.f;
	float TargetShieldPercent = 0.f;

	bool bIsHpBarInitialized = false;
	bool bIsShieldBarInitialized = false;

private:

	UPROPERTY(meta = (BindWidget))
    class UImage* ClearImage;

	UPROPERTY(meta = (BindWidget))
    class UTextBlock* HpText;
	
	UPROPERTY(meta = (BindWidget))
    class UTextBlock* ShieldText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* BossNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* HpTotalText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* ShieldTotalText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* HPProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* ShieldProgressBar;

public:

	void InitBossHp(int32 SetMaxHp);
	void InitBossShield(int32 SetMaxShield);
	void SetBossName(const FString& SetBossName);

	void ChangeMaxHp(int32 AddMaxHp);
	void ChangeCurrentHp(int32 AddHpValue);

	void ChangeMaxShield(int32 AddMaxShield);
	void ChangeCurrentShield(int32 AddShieldValue);

	UFUNCTION()
	void ShowClearImage();

private:
	void RefreshHpBar();
	void RefreshShieldBar();
	void UpdateProgressBars(float InDeltaTime);
	void UpdateShieldVisibility();
	void SnapHpBarToTarget();
	void SnapShieldBarToTarget();
	float GetHpPercent() const;
	float GetShieldPercent() const;

};
