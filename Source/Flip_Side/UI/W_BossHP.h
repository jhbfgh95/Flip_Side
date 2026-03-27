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

	bool IsHpAnimating;
	bool IsShieldAnimating;

	float AnimTime;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	float Duration = 0.5f;

	float StartPercent;
	float TargetPercent;

private:
	int32 MaxHp;
	int32 CurrentHp;

	int32 MaxShield;
	int32 CurrentShield;

	private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShieldProgressBar;

private:
	void SetBossHpPercentage(float Perecetage);
	void SetBossShieldPercentage(float Perecetage);

	void StartHpAnimation();
	void StartShieldAnimation();


	public:

	void InitBossHp(int32 SetMaxHp);
	void InitBossShield(int32 SetMaxShield);

	void ChangeMaxHp(int32 AddMaxHp);
	void ChangeCurrentHp(int32 AddHpValue);

	void ChangeMaxShield(int32 AddMaxShield);
	void ChangeCurrentShield(int32 AddShieldValue);

};
