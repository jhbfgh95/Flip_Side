// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	
	UMaterialInstanceDynamic* HpMID;
	UMaterialInstanceDynamic* ShieldMID;
private:
	
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	TArray<FLinearColor> HpLevelColor;

	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess))
	TArray<FLinearColor> ShieldLevelColor;

	int32 ElementHp = 1;
	int32 ElementShield = 1;
private:
	void SetHpMaterial(int32 HpLevel);

	void SetShieldMaterial(int32 HpLevel);
public:
	void InitElementHp(int32 HpValue);

	void InitElementShield(int32 ShieldValue);

	void DecreaseHp();
	
	void IncreaseHp();

	void DecreaseShield();
	void IncreaseShield();
};
