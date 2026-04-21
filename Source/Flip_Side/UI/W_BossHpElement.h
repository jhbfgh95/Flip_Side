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
	
	UMaterialInstanceDynamic* MID;
private:
	
	UPROPERTY(EditAnywhere, meta =(BindWidget))
	TArray<FLinearColor> HpLevelColor;

	int32 ElementHp = 1;
private:
	void SetHpMaterial(int32 HpLevel);

public:
	void InitElementHp(int32 HpValue);

	void DecreaseHp();
	
	void IncreaseHp();
};
