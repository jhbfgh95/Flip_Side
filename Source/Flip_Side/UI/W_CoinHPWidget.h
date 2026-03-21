// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CoinHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CoinHPWidget : public UUserWidget
{
	GENERATED_BODY()
	

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	UPROPERTY(meta = (BindWidget))
	class UImage* HpImage;

	UMaterialInstanceDynamic* MID;

private:
	float MaxHp = 0;

	float CurrentHp = 0;
	
private:
	bool IsHpAnimating;
	float AnimTime;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	float Duration = 0.5f;

	float StartHpPercent;
	float TargetHpPercent;

private:

	void StartHpAnimation();

	void SetHpPrgressBar(float Percentage);
	void SetMaxHp(float MaxHpValue);
	
	void SetCurrentHp(float Hpvalue);
	
public:
	void InitHpWidget(float MaxHpValue);
	void ChangeMaxHp(float Hpvalue);
	void ChangeCurrentHp(float Hpvalue);
};
