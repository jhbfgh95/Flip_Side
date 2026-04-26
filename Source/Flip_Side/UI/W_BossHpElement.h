// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "W_BossHpElement.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_BossHpElement : public UUserWidget
{
	GENERATED_BODY()
	

	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta = (BindWidget))
	class UImage* ElementImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* ShieldElementImage;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* HpDamageImage;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* ShieldDamageImage;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HPBorder;

	UPROPERTY(meta = (BindWidget))
	class UWidget* ShieldBorder;
	
	UMaterialInstanceDynamic* HpMID;
	UMaterialInstanceDynamic* ShieldMID;
	UMaterialInstanceDynamic* HpDamageMID;
	UMaterialInstanceDynamic* ShieldDamageMID;
private:
	
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	TArray<FLinearColor> HpLevelColor;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	TArray<FLinearColor> ShieldLevelColor;

	UPROPERTY(EditAnywhere, Category = "BossHP|Animation", meta = (AllowPrivateAccess))
	float DamageRemainTime = 0.25f;

	int32 ElementHp = 1;
	int32 ElementShield = 0;

	int32 PendingHpDecreaseCount = 0;
	int32 PendingShieldDecreaseCount = 0;

	FTimerHandle HpDamageTimerHandle;
	FTimerHandle ShieldDamageTimerHandle;
private:
	void SetHpMaterial(int32 HpLevel);

	void SetShieldMaterial(int32 HpLevel);
	void SetHpDamageMaterial(int32 HpLevel);
	void SetShieldDamageMaterial(int32 ShieldLevel);

	void ShowImage(class UImage* Image);
	void HideImage(class UImage* Image);
	void ShowWidget(class UWidget* Widget);
	void HideWidget(class UWidget* Widget);
	void RefreshShieldVisibility();

	void ApplyPendingHpDecrease();
	void ApplyPendingShieldDecrease();
public:
	void InitElementHp(int32 HpValue);

	void InitElementShield(int32 ShieldValue);

	void DecreaseHp();
	
	void IncreaseHp();

	void DecreaseShield();
	void IncreaseShield();
};
