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
	class UShopCoinWSubsystem* CoinWSubsystem;

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
	class UTexture2D* WeaponTexture;

	//코인 면 설정
	UFUNCTION()
	void SelectWeapon();
	
	void InitButton(int32 ID);
	UFUNCTION()
	void HoverWeapon();
	UFUNCTION()
	void UnHoverWeapon();
};
