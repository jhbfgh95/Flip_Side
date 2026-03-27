// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "W_SelectWeaponButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_SelectWeaponButton : public UUserWidget
{
	GENERATED_BODY()
	

private:
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;
	
	class UShopWeaponDataWSubsystem* WeaponDataSubSystem;

	class UDataManagerSubsystem* DataManager;
	
protected:
	virtual void NativeConstruct() override;
public:

	FFaceData WeaponData;

	UPROPERTY(EditAnywhere, Category = "Info")
	FString WeaponName;

	//아이디
    UPROPERTY(meta = (BindWidget))
    class UButton* WeaponButton;

	
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeaponNameBlock;

	UPROPERTY(EditAnywhere, Category="Button")
	class UTexture2D* WeapoTexture;

    UPROPERTY(EditAnywhere, Category = "UI")
    class UMaterialInterface* MaskMaterialBase; 

    UPROPERTY()
    class UMaterialInstanceDynamic* DynamicMaskMaterial;


	//함수
	//class UW_CoinCreateWidget* CoinCreateWidget;

	//코인 면 설정
	UFUNCTION()
	void SelectWeapon();
	
	void InitButton(int32 ID);


	//찾는 버튼인지 판단
	bool IsFindWeapon(FText FindText);
};
